#include "BetterInfoCache.h"
#include "BetterInfoOnline.h"
#include "../utils.hpp"

#include "../layers/FoundListsPopup.h"
#include "Geode/utils/general.hpp"
#include <shared_mutex>

bool BetterInfoCache::init(){
    if(!BaseJsonManager::init("cache.json")) return false;
    doSave();
    establishCachedDicts();
    return true;
}

void BetterInfoCache::finishLoading(){
    auto now = BetterInfo::timeInMs();

    checkClaimableLists();
    cacheRatedLists();

    cacheFollowedCreators();

    log::debug("Finished 2nd stage loading BetterInfoCache in {} ms", BetterInfo::timeInMs() - now);

    std::thread([this] {
        thread::setName("BI 3rd Stage Sleep");
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(120000ms);

        Loader::get()->queueInMainThread([this] {
            auto now = BetterInfo::timeInMs();
            log::debug("Starting 3rd stage loading BetterInfoCache");

            auto GLM = GameLevelManager::sharedState();
            checkLevelsFromDict(GLM->m_onlineLevels);
            checkLevelsFromDict(GLM->m_dailyLevels);

            log::debug("Finished 3rd stage loading BetterInfoCache in {} ms", BetterInfo::timeInMs() - now);
        });
    }).detach();

    
}

void BetterInfoCache::validateLoadedData() {
    validateIsObject("level-name-dict");
    validateIsObject("coin-count-dict");
    validateIsObject("demon-difficulty-dict");
    validateIsObject("username-dict");
    validateIsObject("upload-date-dict");
    validateIsObject("level-info-dict");
    validateIsObject("list-info-dict");
    validateIsObject("user-info-dict");
}

void BetterInfoCache::establishCachedDicts(){
    std::shared_lock guard(m_jsonMutex);
    std::unique_lock lock(m_coinCountsMutex);

    for(auto [key, value] : m_json["coin-count-dict"].as_object()) {
        if(value.is_number()) m_coinCounts[BetterInfo::stoi(key)] = value.as_int();
    }
}

BetterInfoCache::BetterInfoCache(){}

void BetterInfoCache::cacheFollowedCreators() {
    std::unordered_set<int> followedCreators;
    for(auto [key, obj] : CCDictionaryExt<gd::string, CCString*>(GameLevelManager::sharedState()->m_followedCreators)) {
        followedCreators.insert(BetterInfo::stoi(key));
    }

    std::thread([this, followedCreators = std::move(followedCreators)] {
        thread::setName("BI Followed Creators Cache");

        for(auto accountID : followedCreators) {
            if(objectExists("user-info-dict", std::to_string(accountID))) {
                std::shared_lock guard(m_jsonMutex);
                if(m_json["user-info-dict"][std::to_string(accountID)].contains("special")) continue;
            };
            
            Loader::get()->queueInMainThread([accountID] {
                BetterInfoOnline::sharedState()->loadScores(accountID, false, nullptr, nullptr);
            });

            using namespace std::chrono_literals;
            std::this_thread::sleep_for(5000ms);
        }
    }).detach();
}

void BetterInfoCache::cacheScoresResult(CCArray* scores) {
    auto BIOnline = BetterInfoOnline::sharedState();

    auto GLM = GameLevelManager::sharedState();
    for(auto score : CCArrayExt<GJUserScore*>(scores)) {
        if(!GLM->m_followedCreators->objectForKey(std::to_string(score->m_accountID))) continue;

        cacheScore(score);
    }
}

void BetterInfoCache::cacheScore(GJUserScore* score) {
    auto object = matjson::Object();
    object["username"] = std::string(score->m_userName);
    object["icon-type"] = (int) score->m_iconType;
    object["iconID"] = score->m_iconID;
    object["color-1"] = score->m_color1;
    object["color-2"] = score->m_color2;
    object["special"] = score->m_special;
    object["accountID"] = score->m_accountID;
    object["cached"] = std::time(nullptr);

    std::thread([this, object, accountID = score->m_accountID] {
        thread::setName("BI Score Cache");

        std::unique_lock guard(m_jsonMutex);
        m_json["user-info-dict"][std::to_string(accountID)] = object;
        guard.unlock();
        doSave();
    }).detach();
    storeUserName(score->m_userID, score->m_userName);
}

GJUserScore* BetterInfoCache::getCachedOrPlaceholderScore(int accountID) {
    auto score = getCachedScore(accountID);
    if(score) return score;

    auto GLM = GameLevelManager::sharedState();

    score = GJUserScore::create();
    score->m_userName = GLM->tryGetUsername(accountID);
    score->m_iconType = IconType::Cube;
    score->m_iconID = 1;
    score->m_color1 = 0;
    score->m_color2 = 0;
    score->m_special = 0;
    score->m_accountID = accountID;
    return score;
}

GJUserScore* BetterInfoCache::getCachedScore(int accountID) {
    auto idString = std::to_string(accountID);
    if(!objectExists("user-info-dict", idString)) return nullptr;

    std::shared_lock guard(m_jsonMutex);
    auto it = m_json["user-info-dict"].as_object().find(idString);
    if(it == m_json["user-info-dict"].as_object().end()) return nullptr;

    auto object = it->second.as_object();
    if(!object.contains("username") || !object.contains("icon-type") || !object.contains("iconID") || !object.contains("color-1") || !object.contains("color-2") || !object.contains("accountID")) return nullptr;
    if(!object["username"].is_string() || !object["icon-type"].is_number() || !object["iconID"].is_number() || !object["color-1"].is_number() || !object["color-2"].is_number() || !object["accountID"].is_number()) return nullptr;

    auto score = GJUserScore::create();
    score->m_userName = object["username"].as_string();
    score->m_iconType = (IconType) object["icon-type"].as_int();
    score->m_iconID = object["iconID"].as_int();
    score->m_color1 = object["color-1"].as_int();
    score->m_color2 = object["color-2"].as_int();
    score->m_accountID = object["accountID"].as_int();

    if(!object.contains("special") || !object["special"].is_number()) return score;

    score->m_special = object["special"].as_int();
    return score;
}

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

                    //811 is the first rated list, GDPS users can cope
                    if(objectExists("list-info-dict", idString) && objectExists("list-info-dict", "811")) {
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

    auto name = std::string(list->m_listName);
    for(auto it = name.begin(); it < name.end(); it++) {
        if(*it < 0x20) *it = ' ';
    }

    m_json["list-info-dict"][idString]["name"] = name;
    m_json["list-info-dict"][idString]["levels"] = std::vector<int>(list->m_levels);
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

    std::vector<int> toDownload;
    for(auto [listID, _] : m_claimableLists) {
        if(_ != nullptr) continue;

        toDownload.push_back(listID);
    }

    std::thread([this, toDownload = std::move(toDownload)] {
        for(auto listID : toDownload) {
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

            using namespace std::chrono_literals;
            std::this_thread::sleep_for(1000ms);
        }
    }).detach();
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
        if(list) lists->addObject(list);
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
    auto dictExt = CCDictionaryExt<gd::string, GJGameLevel*>(dict);

    auto levels = std::make_shared<std::vector<Ref<GJGameLevel>>>();
    for(auto [key, level] : dictExt) {
        levels->push_back(level);
    }

    std::thread([this, levels]{
        thread::setName("BI Level Cache Check");

        std::set<int> toDownloadRated;
        std::set<int> toDownloadUnrated;

        auto GLM = GameLevelManager::sharedState();

        size_t i = 0;
        cocos2d::CCDictElement* element = nullptr;
        for(auto currentLvl : *levels) {
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

        Loader::get()->queueInMainThread([levels] {
            log::info("Finished checking {} levels from dict", levels->size());
        });
    }).detach();
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