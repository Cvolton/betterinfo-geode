#include "BetterInfoCache.h"
#include "../utils.hpp"

#include <fstream>

bool BetterInfoCache::init(){
    if(!BaseJsonManager::init("cache.json")) return false;
    checkDailies();
    return true;
}

void BetterInfoCache::validateLoadedData() {
    validateIsObject("level-name-dict");
    validateIsObject("coin-count-dict");
}

BetterInfoCache::BetterInfoCache(){}

void BetterInfoCache::checkDailies() {
    std::set<int> toDownload;

    auto GLM = GameLevelManager::sharedState();

    auto dailyLevels = GLM->m_dailyLevels;
    CCDictElement* obj;
    CCDICT_FOREACH(dailyLevels, obj){
        auto currentLvl = static_cast<GJGameLevel*>(obj->getObject());
        if(currentLvl == nullptr) continue;

        auto idString = std::to_string(currentLvl->m_levelID);
        if(objectExists("level-name-dict", idString) && objectExists("coin-count-dict", idString)) continue;

        auto levelFromSaved = static_cast<GJGameLevel*>(GLM->m_onlineLevels->objectForKey(std::to_string(currentLvl->m_levelID).c_str()));
        if(levelFromSaved != nullptr) cacheLevel(levelFromSaved);
        else toDownload.insert(currentLvl->m_levelID);
    }

    if(!toDownload.empty()) cacheLevels(toDownload);
    doSave();
}

void BetterInfoCache::cacheLevel(GJGameLevel* level) {
    auto idString = std::to_string(level->m_levelID);
    m_json["level-name-dict"][idString] = std::string(level->m_levelName);
    m_json["coin-count-dict"][idString] = level->m_coins;
}

void BetterInfoCache::cacheLevels(std::set<int> toDownload) {
    //Search type 10 currently does not have a limit on level IDs, so we can do this all in one request
    bool first = true;
    std::stringstream levels;
    std::vector<std::string> levelSets;
    int i = 0;
    for(const auto& id : toDownload) {
        if(i != 0) levels << ",";
        levels << id;
        first = false;
        
        i = (i + 1) % 300;
        if(i == 0) {
            levelSets.push_back(levels.str());
            levels.str("");
        }
    }

    levelSets.push_back(levels.str());

    //Splitting up the request like this is required because GJSearchObject::create crashes if str is too long
    for(const auto& set : levelSets) {
        auto searchObj = GJSearchObject::create(SearchType::MapPackOnClick, set);
        auto GLM = GameLevelManager::sharedState();
        GLM->m_onlineListDelegate = this;
        GLM->getOnlineLevels(searchObj);
    }

}

std::string BetterInfoCache::getLevelName(int levelID) {
    auto idString = std::to_string(levelID);
    if(!objectExists("level-name-dict", idString)) return "Unknown";

    try {
        return m_json["level-name-dict"][idString].as_string();
    } catch(std::exception) {
        return "Unknown (malformed JSON)";
    }
}

int BetterInfoCache::getCoinCount(int levelID) {
    auto idString = std::to_string(levelID);
    if(!objectExists("coin-count-dict", idString)) return 3;

    try {
        return m_json["coin-count-dict"][idString].as_int();
    } catch(std::exception) {
        return 3;
    }
}

void BetterInfoCache::loadListFinished(cocos2d::CCArray* levels, const char*) {
    for(size_t i = 0; i < levels->count(); i++) {
        auto level = static_cast<GJGameLevel*>(levels->objectAtIndex(i));
        if(level == nullptr) continue;

        cacheLevel(level);
    }

    doSave();
}

void BetterInfoCache::loadListFailed(const char*) {}
void BetterInfoCache::setupPageInfo(std::string, const char*) {}