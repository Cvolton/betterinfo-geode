#include "BetterInfoCache.h"
#include "../utils.hpp"

#include <fstream>
#include <Geode/utils/web.hpp>

bool BetterInfoCache::init(){
    if(!BaseJsonManager::init("cache.json")) return false;
    checkDailies();
    return true;
}

void BetterInfoCache::validateLoadedData() {
    validateIsObject("level-name-dict");
    validateIsObject("coin-count-dict");
    validateIsObject("username-dict");
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

void BetterInfoCache::storeUserName(int userID, std::string username) {
    auto idString = std::to_string(userID);
    log::info("Storing username {} for user ID {}", username, idString);
    m_json["username-dict"][idString] = username;
    doSave();
}

std::string BetterInfoCache::getUserName(int userID) {
    auto idString = std::to_string(userID);
    if(!objectExists("username-dict", idString)) {
        //if gdhistory was faster, this could be sync and the feature would be more efficient, sadly gdhistory is not faster
        web::AsyncWebRequest().fetch(fmt::format("https://history.geometrydash.eu/api/v1/user/{}/brief/", userID)).text().then([userID](const std::string& userData){
            log::info("{}", userData);
            //GEODE_UNWRAP_INTO(auto data, userData);
            auto data = json::parse(userData);
            std::string username;

            if(data["non_player_username"].is_string()) username = data["non_player_username"].as_string();
            else if(data["username"].is_string()) username = data["username"].as_string();

            BetterInfoCache::sharedState()->storeUserName(userID, username);
        }).expect([](const std::string& error){

        });
        return "";
    }

    try {
        return m_json["username-dict"][idString].as_string();
    } catch(std::exception e) {
        log::error("Exception in getUserName: {}", e.what());
        return "";
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