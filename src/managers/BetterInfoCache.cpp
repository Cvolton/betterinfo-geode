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

    arc::spawn(
        cacheFollowedCreators()
    );
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