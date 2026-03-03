#include "BetterInfoCache.h"

BetterInfoCache* BetterInfoCache::sharedState() {
    static BetterInfoCache instance;
    return &instance;
}

void BetterInfoCache::cacheFollowedCreators() {
    /*auto followedCreators = GameStatsManager::sharedState()->m_followedCreators->asExt();
    for(auto [id, _] : followedCreators) {
        auto idNum = BetterInfo::stoi(id);
        if(m_userScoreCache.contains(idNum)) continue;

        this->cacheUserScore(idNum);
    }*/
}