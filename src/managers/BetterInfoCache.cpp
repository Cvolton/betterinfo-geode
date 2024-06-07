#include "BetterInfoCache.h"
#include "../utils.hpp"

#include <Geode/utils/web.hpp>

bool BetterInfoCache::init(){
    if(!BaseJsonManager::init("cache.json")) return false;
    doSave();
    establishCachedDicts();
    return true;
}

void BetterInfoCache::finishLoading(){
    auto GLM = GameLevelManager::sharedState();
    checkLevelsFromDict(GLM->m_onlineLevels);
    checkLevelsFromDict(GLM->m_dailyLevels);
}

void BetterInfoCache::validateLoadedData() {
    validateIsObject("level-name-dict");
    validateIsObject("coin-count-dict");
    validateIsObject("demon-difficulty-dict");
    validateIsObject("username-dict");
    validateIsObject("upload-date-dict");
    validateIsObject("level-info-dict");
}

void BetterInfoCache::establishCachedDicts(){
    std::shared_lock guard(m_jsonMutex);
    std::unique_lock lock(m_coinCountsMutex);

    for(auto [key, value] : m_json["coin-count-dict"].as_object()) {
        if(value.is_number()) m_coinCounts[BetterInfo::stoi(key)] = value.as_int();
    }
}

BetterInfoCache::BetterInfoCache(){}

void BetterInfoCache::checkLevelsFromDict(CCDictionary* dict) {
    std::set<int> toDownloadRated;
    std::set<int> toDownloadUnrated;

    auto GLM = GameLevelManager::sharedState();

    size_t i = 0;
    cocos2d::CCDictElement* element = nullptr;
    CCDICT_FOREACH(dict, element) {
        auto key = element->getStrKey();
        auto currentLvl = static_cast<GJGameLevel*>(element->getObject());
        auto idString = std::to_string(currentLvl->m_levelID);
        if(objectExists("level-name-dict", idString) && objectExists("coin-count-dict", idString) && objectExists("demon-difficulty-dict", idString) && getLevelName(currentLvl->m_levelID) != "") continue;

        auto levelFromSaved = LevelUtils::getLevelFromSaved(currentLvl->m_levelID);
        if(levelFromSaved != nullptr && std::string(levelFromSaved->m_levelName) != "") cacheLevel(levelFromSaved);
        else {
            if(levelFromSaved && levelFromSaved->m_stars > 0) toDownloadRated.insert(currentLvl->m_levelID);
            else toDownloadUnrated.insert(currentLvl->m_levelID);
        }
    }

    if(!toDownloadRated.empty()) cacheLevels(toDownloadRated, SearchType::MapPackOnClick, 2500);
    if(!toDownloadUnrated.empty()) cacheLevels(toDownloadUnrated, SearchType::Type26, 100);

    log::debug("Caching {} rated and {} unrated levels", toDownloadRated.size(), toDownloadUnrated.size());
}

void BetterInfoCache::cacheLevel(GJGameLevel* level) {
    auto idString = std::to_string(level->m_levelID);

    auto name = std::string(level->m_levelName);
    for(auto it = name.begin(); it < name.end(); it++) {
        if(*it < 0x20) *it = ' ';
    }

    //std::cout <<  ("Locking unique_lock cacheLevel") << std::endl;
    std::unique_lock guard(m_jsonMutex);
    m_json["level-name-dict"][idString] = name;
    m_json["coin-count-dict"][idString] = level->m_coins;
    m_json["demon-difficulty-dict"][idString] = level->m_demonDifficulty;
    //std::cout <<  ("Unlocking unique_lock cacheLevel") << std::endl;
}

void BetterInfoCache::cacheLevels(std::set<int> toDownload, SearchType searchType, int levelsPerRequest) {
    //Search type 10 currently does not have a limit on level IDs, so we can do this all in one request
    bool first = true;
    std::stringstream levels;
    std::vector<std::string> levelSets;
    int i = 0;
    for(const auto& id : toDownload) {
        if(i != 0) levels << ",";
        levels << id;
        first = false;
        
        i = (i + 1) % levelsPerRequest;
        if(i == 0) {
            levelSets.push_back(levels.str());
            levels.str("");
        }
    }

    levelSets.push_back(levels.str());

    //Limit to 20 requests, so we don't get too rate limited
    if(levelSets.size() > 20) levelSets.resize(20);

    Loader::get()->queueInMainThread([this, levelSets = std::move(levelSets), searchType]() mutable {
        //Splitting up the request like this is required because GJSearchObject::create crashes if str is too long
        //Status update: This was actually GLM getOnlineLevels, the issue is with CCDict max key length
        //However we still need to split this up because we can request too many levels
        //and we now use search types that do have a total level limit
        auto searchObjects = std::make_shared<std::vector<Ref<GJSearchObject>>>();
        for(const auto& set : levelSets) {
            auto searchObj = GJSearchObject::create(searchType, set);
            searchObjects->push_back(searchObj);
        }

        std::thread([this, searchObjects]() mutable {
            thread::setName("Level Cache");

            using namespace std::chrono_literals;
            for(const auto& searchObj : *searchObjects) {
                ServerUtils::getOnlineLevels(
                    searchObj, 
                    [this](auto levels, bool) {
                        std::thread([this, levels] {
                            thread::setName("Level Cache (Inner)");
                            for(const auto& level : *levels) {
                                cacheLevel(level);
                            }
                            doSave();
                            Loader::get()->queueInMainThread([levels] {
                                if(levels->size() > 0) log::debug("Cached {} levels", levels->size());
                                // this is just a workaround to make sure the vector only gets destroyed in main thread
                            });
                        }).detach();
                    },
                    false
                );

                std::this_thread::sleep_for(1500ms);
            }
            
            Loader::get()->queueInMainThread(std::move([searchObjects]() mutable {
                if(searchObjects->size() > 0) log::debug("Finished caching levels");
                // this is just a workaround to make sure the vector only gets destroyed in main thread
            }));
        }).detach();
    });

}

std::string BetterInfoCache::getLevelName(int levelID) {
    auto idString = std::to_string(levelID);
    if(!objectExists("level-name-dict", idString)) return "Unknown";

    //std::cout <<  ("Locking shared_lock getLevelName") << std::endl;
    std::shared_lock guard(m_jsonMutex);
    //std::cout <<  ("Unlocking shared_lock getLevelName") << std::endl;
    auto it = m_json["level-name-dict"].as_object().find(idString);
    if(it == m_json["level-name-dict"].as_object().end()) return "Unknown";
    if(!it->second.is_string()) return "Unknown (malformed JSON)";
    return it->second.as_string();
}

int BetterInfoCache::getDemonDifficulty(int levelID) {
    auto idString = std::to_string(levelID);
    if(!objectExists("demon-difficulty-dict", idString)) return 0;

    //std::cout <<  ("Locking shared_lock getDemonDifficulty") << std::endl;
    std::shared_lock guard(m_jsonMutex);
    //std::cout <<  ("Unlocking shared_lock getDemonDifficulty") << std::endl;
    auto it = m_json["demon-difficulty-dict"].as_object().find(idString);
    if(it == m_json["demon-difficulty-dict"].as_object().end()) return 0;
    if(!it->second.is_number()) return 0;
    return it->second.as_int();
}

void BetterInfoCache::storeUserName(int userID, const std::string& username) {
    if(username.empty()) {
        return;
    }

    std::thread([this, userID, username] {
        thread::setName("BI Username Store");

        //std::cout <<  ("Locking unique_lock storeUserName") << std::endl;
        std::unique_lock guard(m_jsonMutex);
        auto idString = std::to_string(userID);
        m_json["username-dict"][idString] = username;
        guard.unlock();
        //std::cout <<  ("Unlocking unique_lock storeUserName") << std::endl;
        doSave();

        Loader::get()->queueInMainThread([]() {
            auto levelBrowserLayer = getChildOfType<LevelBrowserLayer>(CCDirector::sharedDirector()->getRunningScene(), 0);
            if(levelBrowserLayer) BetterInfo::reloadUsernames(levelBrowserLayer);
        });
    }).detach();
}

void BetterInfoCache::storeLevelInfo(int levelID, const std::string& field, const std::string& value) {
    if(field.empty() || value.empty()) {
        return;
    }

    //std::cout <<  ("Locking unique_lock storeLevelInfo") << std::endl;
    std::unique_lock guard(m_jsonMutex);
    auto idString = std::to_string(levelID);
    if(!m_json["level-info-dict"][idString].is_object()) m_json["level-info-dict"][idString] = matjson::Object();

    m_json["level-info-dict"][idString][field] = value;
    guard.unlock();
    //std::cout <<  ("Unlocking unique_lock storeLevelInfo") << std::endl;
    doSave();
}

std::string BetterInfoCache::getLevelInfo(int levelID, const std::string& field) {
    //std::cout <<  ("Locking shared_lock getLevelInfo") << std::endl;
    std::shared_lock guard(m_jsonMutex);
    //std::cout <<  ("Unlocking shared_lock getLevelInfo") << std::endl;
    
    auto idString = std::to_string(levelID);
    if(m_json["level-info-dict"].as_object().find(idString) == m_json["level-info-dict"].as_object().end()) return "";

    auto it = m_json["level-info-dict"][idString].as_object().find(field);
    if(it == m_json["level-info-dict"][idString].as_object().end() || !(it->second.is_string())) return "";

    return m_json["level-info-dict"][idString][field].as_string();
}

void BetterInfoCache::storeDatesForLevel(GJGameLevel* level) {
    std::string uploadDateStd = level->m_uploadDate;
    std::string updateDateStd = level->m_updateDate;

    if(!uploadDateStd.empty()) storeLevelInfo(level->m_levelID, "upload-date", uploadDateStd);
    if(!updateDateStd.empty()) storeLevelInfo(level->m_levelID, "update-date", updateDateStd);
}

std::string BetterInfoCache::getUserName(int userID, bool download) {
    static std::mutex downloadingMutex;
    static std::condition_variable downloadingCV;
    static bool downloading = false;
    if(userID == 0) return ""; //this prevents the request from being sent on every account comments load

    auto idString = std::to_string(userID);
    if(!objectExists("username-dict", idString)) {
        if(download && m_attemptedUsernames.find(userID) == m_attemptedUsernames.end()) {
            std::thread([userID] {
                thread::setName("BI Username Downloader");

                std::unique_lock lock(downloadingMutex);
                downloadingCV.wait(lock, []{ return !downloading; });
                downloading = true;

                /*web::AsyncWebRequest().fetch(fmt::format("https://history.geometrydash.eu/api/v1/user/{}/brief/", userID)).json().then([userID](const matjson::Value& data){
                    log::debug("Restored green username for {}: {}", userID, data.dump(matjson::NO_INDENTATION));
                    std::string username;

                    if(data["non_player_username"].is_string()) username = data["non_player_username"].as_string();
                    else if(data["username"].is_string()) username = data["username"].as_string();

                    BetterInfoCache::sharedState()->storeUserName(userID, username);
                    downloading = false;
                    downloadingCV.notify_one();
                }).expect([userID](const std::string& error){
                    log::error("Error while getting username for {}: {}", userID, error);
                    downloading = false;
                    downloadingCV.notify_one();
                });*/
            }).detach();
            m_attemptedUsernames.insert(userID);
        }
        return "";
    }

    //std::cout <<  ("Locking shared_lock getUserName") << std::endl;
    std::shared_lock guard(m_jsonMutex);
    //std::cout <<  ("Unlocking shared_lock getUserName") << std::endl;
    auto it = m_json["username-dict"].as_object().find(idString);
    if(it == m_json["username-dict"].as_object().end()) return "";
    if(!it->second.is_string()) return "";
    return it->second.as_string();
}

int BetterInfoCache::getCoinCount(int levelID) {
    //std::cout <<  ("Locking shared_lock getCoinCount") << std::endl;
    std::shared_lock guard(m_jsonMutex);
    std::unique_lock lock(m_coinCountsMutex);
    //std::cout <<  ("Unlocking shared_lock getCoinCount") << std::endl;
    if(m_coinCounts.find(levelID) != m_coinCounts.end()) return m_coinCounts.at(levelID);

    auto idString = std::to_string(levelID);
    if(!objectExists("coin-count-dict", idString)) return 3;

    auto it = m_json["coin-count-dict"].as_object().find(idString);
    if(it == m_json["coin-count-dict"].as_object().end()) return 3;
    if(!it->second.is_number()) return 3;
    return m_coinCounts[levelID] = it->second.as_int();
}

void BetterInfoCache::storeUploadDate(int levelID, const std::string& date) {
    if(date.empty()) {
        return;
    }

    //std::cout <<  ("Locking unique_lock storeUploadDate") << std::endl;
    std::unique_lock guard(m_jsonMutex);
    auto idString = std::to_string(levelID);
    m_json["upload-date-dict"][idString] = date;
    guard.unlock();
    //std::cout <<  ("Unlocking unique_lock storeUploadDate") << std::endl;
    doSave();

    if(m_uploadDateDelegate) m_uploadDateDelegate->onUploadDateLoaded(levelID, date);
    m_uploadDateDelegate = nullptr;
}

std::string BetterInfoCache::getUploadDate(int levelID, UploadDateDelegate* delegate) {
    if(levelID == 0) return "";
    m_uploadDateDelegate = delegate;

    auto idString = std::to_string(levelID);
    if(!objectExists("upload-date-dict", idString)) {
        if(m_attemptedLevelDates.find(levelID) == m_attemptedLevelDates.end()) {
            /*web::AsyncWebRequest().fetch(fmt::format("https://history.geometrydash.eu/api/v1/date/level/{}/", levelID)).json().then([levelID](const matjson::Value& data){
                if(!data["approx"].is_object()) return;
                if(!data["approx"]["estimation"].is_string()) return;

                BetterInfoCache::sharedState()->storeUploadDate(levelID, data["approx"]["estimation"].as_string());
            }).expect([levelID](const std::string& error){
                log::error("Error while getting exact upload date for level {}: {}", levelID, error);
            });*/
            m_attemptedLevelDates.insert(levelID);
        }
        return "";
    }

    //std::cout <<  ("Locking unique_lock getUploadDate") << std::endl;
    std::shared_lock guard(m_jsonMutex);
    //std::cout <<  ("Unlocking unique_lock getUploadDate") << std::endl;
    auto it = m_json["upload-date-dict"].as_object().find(idString);
    if(it == m_json["upload-date-dict"].as_object().end()) return "";
    if(!it->second.is_string()) return "";
    return it->second.as_string();
}