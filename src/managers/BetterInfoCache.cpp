#include "BetterInfoCache.h"
#include "BetterInfoOnline.h"
#include <arc/time/Sleep.hpp>

BetterInfoCache* BetterInfoCache::sharedState() {
    static BetterInfoCache instance;
    return &instance;
}

BetterInfoCache::BetterInfoCache() {
    //TODO load json
}

void BetterInfoCache::startLoading() {
    //TODO block if json is not loaded

    if(m_loadingStarted) return;
    m_loadingStarted = true;

    arc::spawn([this] -> arc::Future<> {
        co_await cacheSavedLevels();
        co_await cacheFollowedCreators();
    });
}

arc::Future<> BetterInfoCache::cacheFollowedCreators() {
    log::debug("Started caching followed creators");
    std::vector<int> followedCreatorIDs;
    co_await waitForMainThread([this, &followedCreatorIDs] {
        auto followedCreators = GameLevelManager::sharedState()->m_followedCreators->asExt();
        for(auto [id, _] : followedCreators) {
            auto idNum = BetterInfo::stoi(id);
            if(m_userScoreCache.contains(idNum)) continue;

            followedCreatorIDs.push_back(idNum);
        }
    });

    for(auto id : followedCreatorIDs) {
        co_await arc::sleep(asp::Duration::fromSecs(5));
        co_await waitForMainThread([id] {
            log::debug("Caching score for followed creator with ID: {}", id);
            BetterInfoOnline::sharedState()->loadScores(id, false, nullptr);
        });
    }
}

void BetterInfoCache::cacheUserScore(int accountID, GJUserScore* score) {
    m_userScoreCache[accountID] = score;
}

void BetterInfoCache::cacheUserScores(CCArray* scores) {
    for(auto score : CCArrayExt<GJUserScore>(scores)) {
        if(!GameLevelManager::sharedState()->m_followedCreators->valueForKey(std::to_string(score->m_accountID).c_str())) continue;

        cacheUserScore(score->m_accountID, score);
    }
}

GJUserScore* BetterInfoCache::getUserScore(int accountID) {
    if(m_userScoreCache.contains(accountID)) {
        return m_userScoreCache[accountID];
    }

    auto score = GJUserScore::create();
    score->m_accountID = accountID;
    return score;
}

arc::Future<> BetterInfoCache::cacheSavedLevels() {
    co_await this->cacheLevels(GameLevelManager::sharedState()->m_dailyLevels);
    co_await this->cacheLevels(GameLevelManager::sharedState()->m_onlineLevels);
}

arc::Future<> BetterInfoCache::cacheLevels(CCDictionary* levels) {
    std::vector<int> needsDownloadingRated;
    std::vector<int> needsDownloadingUnrated;

    co_await waitForMainThread([this, levels, &needsDownloadingRated, &needsDownloadingUnrated] {
        for(auto [id, level] : levels->asExt<gd::string, GJGameLevel*>()) {
            auto idNum = BetterInfo::stoi(id);
            if(m_levelCache.contains(idNum) || m_levelFailures[idNum] > 5) continue;

            if(!level->m_levelName.empty()) {
                cacheLevel(level);
            } else {
                if(level->m_stars > 0) {
                    needsDownloadingRated.push_back(idNum);
                } else {
                    needsDownloadingUnrated.push_back(idNum);
                }
            }
        }
    });

    log::debug("Caching levels, rated: {}, unrated: {}", needsDownloadingRated.size(), needsDownloadingUnrated.size());

    co_await cacheLevelBatch(std::move(needsDownloadingRated), true);
    co_await cacheLevelBatch(std::move(needsDownloadingUnrated), false);
}

arc::Future<> BetterInfoCache::cacheLevelBatch(std::vector<int> levelIDs, bool rated) {
    std::vector<int> batch;
    while (!levelIDs.empty()) {
        batch.clear();
        
        size_t batchSize = std::min(rated ? (size_t)1000 : (size_t)100, levelIDs.size());
        batch.reserve(batchSize);

        for (size_t i = 0; i < batchSize; i++) {
            batch.push_back(levelIDs.back());
            levelIDs.pop_back();
        }

        log::debug("Caching batch of levels, size: {}, rated: {}", batch.size(), rated);

        GJSearchObject* searchObject = nullptr;
        co_await waitForMainThread([&searchObject, batch = std::move(batch)] {
            searchObject = GJSearchObject::create(
                SearchType::MapPackOnClick, 
                fmt::format("{}", fmt::join(batch, ","))
            );
        });

        ServerUtils::getOnlineLevels(searchObject, [] (std::shared_ptr<std::vector<Ref<GJGameLevel>>> levels, bool success, bool explicitError) {
            if(!success) return;

            for(auto& level : *levels) {
                BetterInfoCache::sharedState()->cacheLevel(level);
            }

            log::debug("Cached batch of levels, size: {}", levels->size());
        });

        co_await arc::sleep(asp::Duration::fromSecs(30));
    }

    co_return;
}

void BetterInfoCache::cacheLevel(GJGameLevel* level) {
    m_levelCache[level->m_levelID] = {
        level->m_levelName,
        level->m_coins,
        level->m_demonDifficulty
    };
}

BetterInfoCache::CachedLevel& BetterInfoCache::getLevel(int levelID) {
    if(m_levelCache.contains(levelID)) {
        return m_levelCache[levelID];
    }

    static CachedLevel emptyLevel = {"", 0, 0};
    return emptyLevel;
}