#include "BetterInfoCache.h"
#include "../utils.hpp"

#include "../layers/FoundListsPopup.h"

bool BetterInfoCache::init(){
    if(!BaseJsonManager::init("cache.json")) return false;
    doSave();
    establishCachedDicts();
    return true;
}

void BetterInfoCache::finishLoading(){
    auto GLM = GameLevelManager::sharedState();
    checkClaimableLists();
    cacheRatedLists();

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
    validateIsObject("list-info-dict");
}

void BetterInfoCache::establishCachedDicts(){
    std::shared_lock guard(m_jsonMutex);
    std::unique_lock lock(m_coinCountsMutex);

    for(auto [key, value] : m_json["coin-count-dict"].as_object()) {
        if(value.is_number()) m_coinCounts[BetterInfo::stoi(key)] = value.as_int();
    }
}

BetterInfoCache::BetterInfoCache(){}

void BetterInfoCache::cacheRatedLists(int page) {
    auto searchObj = GJSearchObject::create(SearchType::Awarded);
    searchObj->m_searchMode = 1;
    searchObj->m_page = page;

    ServerUtils::getLevelLists(
        searchObj, 
        [this, page](auto lists, bool) {
            std::thread([this, lists, page] {
                thread::setName("BI Rated List Cache");

                bool found = lists->empty();
                for(auto list : *lists) {
                    auto idString = std::to_string(list->m_listID);

                    if(objectExists("list-info-dict", idString)) {
                        found = true;
                        break;
                    }
                    cacheList(list);
                }

                using namespace std::chrono_literals;
                std::this_thread::sleep_for(100ms);
                
                
                Loader::get()->queueInMainThread([this, found, page] {
                    if(found) {
                        log::debug("Finished caching rated lists");
                        checkClaimableLists();
                        return;
                    }

                    cacheRatedLists(page + 1);
                });
            }).detach();
        },
        true
    );
}

void BetterInfoCache::checkClaimableLists() {
    std::unordered_set<int> completedLevels;

    auto GLM = GameLevelManager::sharedState();
    for(auto [key, obj] : CCDictionaryExt<gd::string, GJGameLevel*>(GLM->m_onlineLevels)) {
        if(obj->m_normalPercent < 100) continue;
        completedLevels.insert(obj->m_levelID);
    }

    std::thread([this, completedLevels = std::move(completedLevels)] {
        thread::setName("BI Claimable List Checker");

        //log::info("Completed levels {}", completedLevels);

        std::unordered_set<int> completedLists;

        std::shared_lock guard(m_jsonMutex);
        for(auto [key, value] : m_json["list-info-dict"].as_object()) {
            if(!value.is_object()) continue;
            if(!value["diamonds"].is_number()) continue;
            if(!value["levels"].is_array()) continue;
            if(!value["levels-to-claim"].is_number()) continue;

            if(value["diamonds"].as_int() <= 0) continue;
            
            auto listID = BetterInfo::stoi(key);
            auto levels = value["levels"].as_array();
            auto levelsToClaim = value["levels-to-claim"].as_int();

            //log::debug("Checking list {} with {} levels", listID, levels.size());

            int completed = 0;
            for(auto level : levels) {
                if(!level.is_number()) continue;
                if(completedLevels.find(level.as_int()) != completedLevels.end()) completed++;
            }

            //log::info("Completed {} out of {} levels from list {}", completed, levelsToClaim, listID);

            if(completed >= levelsToClaim) {
                completedLists.insert(listID);
            }
        }

        Loader::get()->queueInMainThread([this, completedLists = std::move(completedLists)] {
            for(auto listID : completedLists) {
                auto list = GJLevelList::create();
                list->m_listID = listID;
                if(GameStatsManager::sharedState()->hasClaimedListReward(list)) continue;
                if(m_claimableLists.contains(listID)) continue;

                m_claimableLists.emplace(listID,nullptr);
                log::info("Can claim reward for list {}", listID);
            }

            downloadClaimableLists();
        });
    }).detach();
}

void BetterInfoCache::cacheList(GJLevelList* list) {
    std::unique_lock guard(m_jsonMutex);
    auto idString = std::to_string(list->m_listID);
    if(!m_json["list-info-dict"][idString].is_object()) m_json["list-info-dict"][idString] = matjson::Object();

    m_json["list-info-dict"][idString]["name"] = list->m_listName;
    m_json["list-info-dict"][idString]["levels"] = list->m_levels;
    m_json["list-info-dict"][idString]["levels-to-claim"] = list->m_levelsToClaim;
    m_json["list-info-dict"][idString]["diamonds"] = list->m_diamonds;
    guard.unlock();
    //std::cout <<  ("Unlocking unique_lock storeLevelInfo") << std::endl;
    doSave();
}

void BetterInfoCache::tryShowClaimableListsPopup(CCLayer* scene) {
    if(m_claimableLists.empty()) return;

    auto popup = FoundListsPopup::create();
    popup->m_scene = scene;
    popup->show();
}

size_t BetterInfoCache::claimableListsCount() {
    return m_claimableLists.size();
}

void BetterInfoCache::downloadClaimableLists() {
    if(m_claimableLists.empty()) return;

    for(auto [listID, _] : m_claimableLists) {
        if(_ != nullptr) continue;

        auto searchObj = GJSearchObject::create(SearchType::Search, std::to_string(listID));
        ServerUtils::getLevelLists(
            searchObj, 
            [this](auto lists, bool) {
                if(lists->empty()) return;
                m_claimableLists[lists->at(0)->m_listID] = lists->at(0);
                log::debug("Downloaded list {}", lists->at(0)->m_listID);
            },
            false
        );
    }
}

void BetterInfoCache::showClaimableLists() {
    if(m_claimableLists.empty()) return;

    // this is sort of an ugly workaround
    // and im betting on nobody using this search type
    // which is why the number was generated with random.org lol
    auto searchObj = GJSearchObject::create((SearchType) 212156);
    searchObj->m_searchMode = 1;
    auto lists = CCArray::create();
    for(auto [_, list] : m_claimableLists) {
        lists->addObject(list);
    }
    GameLevelManager::sharedState()->storeSearchResult(lists, fmt::format("{}:{}:{}", lists->count(), 0, lists->count()), searchObj->getKey());

    auto scene = LevelBrowserLayer::scene(searchObj);
    auto transitionFade = CCTransitionFade::create(0.5, scene);
    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void BetterInfoCache::removeClaimedLists() {
    std::unordered_set<int> toErase;

    for(auto [listID, _] : m_claimableLists) {
        if(_ == nullptr) continue;
        if(GameStatsManager::sharedState()->hasClaimedListReward(_)) {
            toErase.insert(listID);
        }
    }

    for(auto listID : toErase) {
        m_claimableLists.erase(listID);
    }
}

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
    static std::optional<web::WebTask> downloadingTask;
    if(userID == 0) return ""; //this prevents the request from being sent on every account comments load

    auto idString = std::to_string(userID);
    if(!objectExists("username-dict", idString)) {
        if(download && m_attemptedUsernames.find(userID) == m_attemptedUsernames.end()) {
            std::thread([userID, this] {
                thread::setName("BI Username Downloader");

                std::unique_lock lock(downloadingMutex);
                downloadingCV.wait(lock, []{ return !downloadingTask.has_value(); });

                downloadingTask = ServerUtils::getBaseRequest().get(fmt::format("https://history.geometrydash.eu/api/v1/user/{}/brief/", userID)).map(
                    [userID, this](web::WebResponse* response) {
                        auto json = response->json();
                        if(!response->ok() || json.isErr()) {
                            log::error("Error while getting username for {}: {} - {}", userID, response->code(), response->string().unwrapOr("No response"));
                            downloadingTask = std::nullopt;
                            downloadingCV.notify_one();
                            return *response;                            
                        }

                        auto data = json.unwrap();
                        std::string username;

                        if(data["non_player_username"].is_string()) username = data["non_player_username"].as_string();
                        else if(data["username"].is_string()) username = data["username"].as_string();

                        storeUserName(userID, username);
                        log::debug("Restored green username for {}: {}", userID, username);
                        downloadingTask = std::nullopt;
                        return *response;
                    }
                );
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

            static std::mutex downloadingMutex;
            static std::unordered_map<int, web::WebTask> tasks;

            std::unique_lock lock(downloadingMutex);

            if(tasks.find(levelID) != tasks.end()) {
                return "";
            }

            tasks.emplace(levelID, ServerUtils::getBaseRequest().get(fmt::format("https://history.geometrydash.eu/api/v1/date/level/{}/", levelID)).map(
                [levelID, this](web::WebResponse* response) {
                    auto json = response->json();
                    if(!response->ok() || json.isErr()) {
                        log::error("Error while getting exact upload date for level {}: {} - {}", levelID, response->code(), response->string().unwrapOr("No response"));
                        return *response;
                    }

                    auto data = json.unwrap();
                    if(!data["approx"].is_object()) return *response;
                    if(!data["approx"]["estimation"].is_string()) return *response;

                    storeUploadDate(levelID, data["approx"]["estimation"].as_string());
                    return *response;
                }
            ));

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