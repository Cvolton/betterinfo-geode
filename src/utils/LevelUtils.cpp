#include "LevelUtils.h"
#include "ServerUtils.h"
#include "../managers/BetterInfoCache.h"

GJGameLevel* LevelUtils::getLevelFromSaved(int levelID) {
    return static_cast<GJGameLevel*>(GameLevelManager::sharedState()->m_onlineLevels->objectForKey(std::to_string(levelID)));
}

std::deque<GJGameLevel*> LevelUtils::completedDeque() {
    std::deque<GJGameLevel*> levelsDeque;
    auto levels = GameLevelManager::sharedState()->m_onlineLevels;
    CCDictElement* obj;
    CCDICT_FOREACH(levels, obj){
        auto currentLvl = static_cast<GJGameLevel*>(obj->getObject());
        levelsDeque.push_back(currentLvl);
    }
    return levelsDeque;
}

void LevelUtils::resetLevelCaches() {
    GameLevelManager::sharedState()->m_timerDict->removeAllObjects();
    ServerUtils::resetCache();
}

int LevelUtils::levelDifficultyAsInt(GJGameLevel* level) {
    if(level->m_demon != 0) return 6;
    if(level->m_autoLevel) return -1;
    return (level->m_ratings == 0) ? 0 : (level->m_ratingsSum / level->m_ratings);
}

int LevelUtils::levelDemonDifficultyAsInt(GJGameLevel* level) {
    int demonDifficulty = 2;
    if(level->m_demonDifficulty >= 5) demonDifficulty = level->m_demonDifficulty - 2;
    else if(level->m_demonDifficulty >= 3) demonDifficulty = level->m_demonDifficulty - 3;
    return demonDifficulty;
}

bool LevelUtils::levelHasCollectedCoins(GJGameLevel* level) {
    auto coinDict = GameStatsManager::sharedState()->m_verifiedUserCoins;
    auto coinDict2 = GameStatsManager::sharedState()->m_pendingUserCoins;
    bool hasAllCoins = true;
    auto coins = level->m_coins ? level->m_coins : BetterInfoCache::sharedState()->getCoinCount(level->m_levelID);
    for(int i = 0; i < coins; i++){
        bool hasntCoin = coinDict->objectForKey(level->getCoinKey(i + 1)) == nullptr && coinDict2->objectForKey(level->getCoinKey(i + 1)) == nullptr;
        if(hasntCoin) hasAllCoins = false;
    }
    return hasAllCoins;
}