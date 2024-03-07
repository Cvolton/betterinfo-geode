#include "BetterInfoCache.h"
#include "../utils.hpp"

#include <Geode/utils/web.hpp>

bool BetterInfoCache::init(){
    if(!BaseJsonManager::init("cache.json")) return false;
    Loader::get()->queueInMainThread([this]() {
        auto GLM = GameLevelManager::sharedState();
        checkLevelsFromDict(GLM->m_onlineLevels);
        checkLevelsFromDict(GLM->m_dailyLevels);
        doSave();
    });
    return true;
}

void BetterInfoCache::validateLoadedData() {
    validateIsObject("level-name-dict");
    validateIsObject("coin-count-dict");
    validateIsObject("demon-difficulty-dict");
    validateIsObject("username-dict");
    validateIsObject("upload-date-dict");
    validateIsObject("level-info-dict");
}

BetterInfoCache::BetterInfoCache(){}

void BetterInfoCache::checkLevelsFromDict(CCDictionary* dict) {
    std::set<int> toDownload;

    auto GLM = GameLevelManager::sharedState();

    for(auto [key, currentLvl] : CCDictionaryExt<gd::string, GJGameLevel*>(dict)) {
        log::info("doing level {}", key);
        auto idString = std::to_string(currentLvl->m_levelID);
        if(objectExists("level-name-dict", idString) && objectExists("coin-count-dict", idString) && objectExists("demon-difficulty-dict", idString) && getLevelName(currentLvl->m_levelID) != "") continue;

        auto levelFromSaved = LevelUtils::getLevelFromSaved(currentLvl->m_levelID);
        if(levelFromSaved != nullptr && std::string(levelFromSaved->m_levelName) != "") cacheLevel(levelFromSaved);
        else toDownload.insert(currentLvl->m_levelID);
    }

    log::info("checkLevelsFromDict for loop done");

    if(!toDownload.empty()) cacheLevels(toDownload);
}

void BetterInfoCache::cacheLevel(GJGameLevel* level) {
    std::lock_guard<std::mutex> guard(m_jsonMutex);
    auto idString = std::to_string(level->m_levelID);
    m_json["level-name-dict"][idString] = std::string(level->m_levelName);
    m_json["coin-count-dict"][idString] = level->m_coins;
    m_json["demon-difficulty-dict"][idString] = level->m_demonDifficulty;
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
        GLM->m_levelManagerDelegate = this;
        GLM->getOnlineLevels(searchObj);
    }

}

std::string BetterInfoCache::getLevelName(int levelID) {
    auto idString = std::to_string(levelID);
    if(!objectExists("level-name-dict", idString)) return "Unknown";

    if(!m_json["level-name-dict"][idString].is_string()) return "Unknown (malformed JSON)";
    return m_json["level-name-dict"][idString].as_string();
}

int BetterInfoCache::getDemonDifficulty(int levelID) {
    auto idString = std::to_string(levelID);
    if(!objectExists("demon-difficulty-dict", idString)) return 0;

    if(!m_json["demon-difficulty-dict"][idString].is_number()) return 0;
    return m_json["demon-difficulty-dict"][idString].as_int();
}

void BetterInfoCache::storeUserName(int userID, const std::string& username) {
    if(username.empty()) {
        return;
    }

    auto idString = std::to_string(userID);
    std::lock_guard<std::mutex> guard(m_jsonMutex);
    m_json["username-dict"][idString] = username;
    doSave();

    auto levelBrowserLayer = getChildOfType<LevelBrowserLayer>(CCDirector::sharedDirector()->getRunningScene(), 0);
    if(levelBrowserLayer) BetterInfo::reloadUsernames(levelBrowserLayer);
}

void BetterInfoCache::storeLevelInfo(int levelID, const std::string& field, const std::string& value) {
    if(field.empty() || value.empty()) {
        return;
    }

    auto idString = std::to_string(levelID);
    std::lock_guard<std::mutex> guard(m_jsonMutex);
    if(!m_json["level-info-dict"][idString].is_object()) m_json["level-info-dict"][idString] = matjson::Object();

    m_json["level-info-dict"][idString][field] = value;
    doSave();
}

std::string BetterInfoCache::getLevelInfo(int levelID, const std::string& field) {
    auto idString = std::to_string(levelID);
    if(!m_json["level-info-dict"][idString].is_object()) m_json["level-info-dict"][idString] = matjson::Object();
    if(!m_json["level-info-dict"][idString][field].is_string()) return "";

    return m_json["level-info-dict"][idString][field].as_string();
}

void BetterInfoCache::storeDatesForLevel(GJGameLevel* level) {
    std::string uploadDateStd = level->m_uploadDate;
    std::string updateDateStd = level->m_updateDate;

    std::lock_guard<std::mutex> guard(m_jsonMutex);
    if(!uploadDateStd.empty()) storeLevelInfo(level->m_levelID, "upload-date", uploadDateStd);
    if(!updateDateStd.empty()) storeLevelInfo(level->m_levelID, "update-date", updateDateStd);
}

std::string BetterInfoCache::getUserName(int userID, bool download) {
    if(userID == 0) return ""; //this prevents the request from being sent on every account comments load

    auto idString = std::to_string(userID);
    if(!objectExists("username-dict", idString)) {
        //if gdhistory was faster, this could be sync and the feature would be more efficient, sadly gdhistory is not faster
        if(download && m_attemptedUsernames.find(userID) == m_attemptedUsernames.end()) {
            web::AsyncWebRequest().fetch(fmt::format("https://history.geometrydash.eu/api/v1/user/{}/brief/", userID)).json().then([userID](const matjson::Value& data){
                log::info("Restored green username for {}: {}", userID, data.dump(matjson::NO_INDENTATION));
                std::string username;

                if(data["non_player_username"].is_string()) username = data["non_player_username"].as_string();
                else if(data["username"].is_string()) username = data["username"].as_string();

                BetterInfoCache::sharedState()->storeUserName(userID, username);
            }).expect([userID](const std::string& error){
                log::error("Error while getting username for {}: {}", userID, error);
            });
            m_attemptedUsernames.insert(userID);
        }
        return "";
    }

    if(!m_json["username-dict"][idString].is_string()) return "";
    return m_json["username-dict"][idString].as_string();
}

int BetterInfoCache::getCoinCount(int levelID) {
    auto idString = std::to_string(levelID);
    if(!objectExists("coin-count-dict", idString)) return 3;

    if(!m_json["coin-count-dict"][idString].is_number()) return 3;
    return m_json["coin-count-dict"][idString].as_int();
}

void BetterInfoCache::storeUploadDate(int levelID, const std::string& date) {
    if(date.empty()) {
        return;
    }

    auto idString = std::to_string(levelID);
    std::lock_guard<std::mutex> guard(m_jsonMutex);
    m_json["upload-date-dict"][idString] = date;
    doSave();

    if(m_uploadDateDelegate) m_uploadDateDelegate->onUploadDateLoaded(levelID, date);
    m_uploadDateDelegate = nullptr;
}

std::string BetterInfoCache::getUploadDate(int levelID, UploadDateDelegate* delegate) {
    if(levelID == 0) return "";
    m_uploadDateDelegate = delegate;

    auto idString = std::to_string(levelID);
    if(!objectExists("upload-date-dict", idString)) {
        //if gdhistory was faster, this could be sync and the feature would be more efficient, sadly gdhistory is not faster
        if(m_attemptedLevelDates.find(levelID) == m_attemptedLevelDates.end()) {
            web::AsyncWebRequest().fetch(fmt::format("https://history.geometrydash.eu/api/v1/date/level/{}/", levelID)).json().then([levelID](const matjson::Value& data){
                if(!data["approx"].is_object()) return;
                if(!data["approx"]["estimation"].is_string()) return;

                BetterInfoCache::sharedState()->storeUploadDate(levelID, data["approx"]["estimation"].as_string());
            }).expect([levelID](const std::string& error){
                log::error("Error while getting exact upload date for level {}: {}", levelID, error);
            });
            m_attemptedLevelDates.insert(levelID);
        }
        return "";
    }

    if(!m_json["upload-date-dict"][idString].is_string()) return "";
    return m_json["upload-date-dict"][idString].as_string();
}

void BetterInfoCache::loadLevelsFinished(cocos2d::CCArray* levels, const char*) {
    for(size_t i = 0; i < levels->count(); i++) {
        auto level = static_cast<GJGameLevel*>(levels->objectAtIndex(i));
        if(level == nullptr) continue;

        cacheLevel(level);
    }

    doSave();
}

void BetterInfoCache::loadLevelsFailed(const char*) {}
void BetterInfoCache::setupPageInfo(std::string, const char*) {}