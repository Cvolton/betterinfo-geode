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

/**
 * User Score Caching
 */
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

/**
 * Level Caching
 */
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

/**
 * Level Date Caching
 */
void BetterInfoCache::fetchLevelDate(int levelID, geode::Function<void(time_t)> callback) {
    if(ServerUtils::isGDPS()) return;
    if(m_levelDateCache.contains(levelID)) {
        callback(m_levelDateCache[levelID]);
        return;
    }

    async::spawn(
        ServerUtils::getBaseRequest().get(fmt::format("https://history.geometrydash.eu/api/v1/date/level/{}/", levelID)),
        [callback = std::move(callback), this](web::WebResponse response) mutable {
            if(!response.ok()) return;

            auto json = response.json().unwrapOrDefault();
            if(!json["approx"].contains("estimation") || !json["approx"].contains("online_id")) return;

            time_t time = TimeUtils::isoStringToTime(json["approx"]["estimation"].asString().unwrapOrDefault());
            m_levelDateCache[json["approx"]["online_id"].asInt().unwrapOrDefault()] = time;
            callback(time);
        }
    );
}

/**
 * Username Caching
 */
void BetterInfoCache::fetchUsername(int userID, geode::Function<void(std::string)> callback) {
    if(ServerUtils::isGDPS() || userID == 0) return;
    if(m_usernameCache.contains(userID)) {
        callback(m_usernameCache[userID]);
        return;
    }

    async::spawn(
        ServerUtils::getBaseRequest().get(fmt::format("https://history.geometrydash.eu/api/v1/user/{}/brief/", userID)),
        [callback = std::move(callback), this](web::WebResponse response) mutable {
            if(!response.ok()) return;

            auto json = response.json().unwrapOrDefault();
            if(!json.contains("username") || !json.contains("user_id")) return;

            std::string username = json["username"].asString().unwrapOrDefault();
            m_usernameCache[json["user_id"].asInt().unwrapOrDefault()] = username;
            callback(username);
        }
    );
}

std::string BetterInfoCache::tryGetUsername(int userID) {
    if(m_usernameCache.contains(userID)) return m_usernameCache[userID];

    fetchUsername(userID, [userID](std::string username) {
        log::debug("Fetched username for user ID {}: {}", userID, username);
    });

    return "";
}

/**
 * Rated List Caching
 */
void BetterInfoCache::cacheRatedListsFromMegaResponse(std::string_view response) {
    if(ServerUtils::isGDPS()) return;

    auto parts = geode::utils::string::split(response, "#");
    if (parts.empty()) return;
    
    auto lists = geode::utils::string::split(parts[0], "|");
    for (const auto& listStr : lists) {
        if (listStr.empty()) continue;
        m_levelLists.push_back(GJLevelList::create(BetterInfo::responseToDict(listStr)));
    }

    checkClaimableLists();
}

void BetterInfoCache::checkClaimableLists() {
    m_claimableLists.clear();

    for (const auto& list : m_levelLists) {
        if (GameStatsManager::get()->hasClaimedListReward(list)) continue;
        if (list->completedLevels() < list->m_levelsToClaim) continue;

        m_claimableLists.push_back(list);
    }
}

size_t BetterInfoCache::claimableListsCount() const {
    return m_claimableLists.size();
}

void BetterInfoCache::showClaimableLists() {
    if(m_claimableLists.empty()) return;

    auto searchObj = GJSearchObject::create((SearchType) 212156);
    searchObj->m_searchMode = 1;
    auto lists = CCArray::create();
    for(auto list : m_claimableLists) {
        if(list) lists->addObject(list);
    }
    GameLevelManager::sharedState()->storeSearchResult(lists, fmt::format("{}:{}:{}", lists->count(), 0, lists->count()), searchObj->getKey());

    auto scene = LevelBrowserLayer::scene(searchObj);
    auto transitionFade = CCTransitionFade::create(0.5, scene);
    CCDirector::sharedDirector()->pushScene(transitionFade);

    if(auto LBL = scene->getChildByType<LevelBrowserLayer>(0)) {
        LBL->m_refreshBtn->setVisible(false);
        if(auto starButton = LBL->getChildByIDRecursive("star-button"_spr)) {
            auto parent = starButton->getParent();
            starButton->removeFromParent();
            parent->updateLayout();
        }
    }
}

/**
 * Vault Code Caching
 */
void BetterInfoCache::cacheVaultCode(const std::string& id, std::string_view code) {
    m_vaultCodeCache[id] = code;
}

std::string BetterInfoCache::getVaultCode(const std::string& id) {
    if(m_vaultCodeCache.contains(id)) return m_vaultCodeCache[id];
    return "";
}