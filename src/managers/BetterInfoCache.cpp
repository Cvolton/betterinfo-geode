#include "BetterInfoCache.h"
#include "BetterInfoOnline.h"
#include "../utils.hpp"

#include "../layers/FoundListsPopup.h"
#include "Geode/utils/general.hpp"
#include <shared_mutex>

bool BetterInfoCache::init(){
    if(!BaseJsonManager::init("cache.json")) return false;
    doSave();
    return true;
}

void BetterInfoCache::finishLoading(){
    auto now = BetterInfo::timeInMs();

    //checkClaimableLists();
    //cacheRatedLists();

    cacheFollowedCreators();
    establishCachedDicts();

    log::debug("Finished 2nd stage loading BetterInfoCache in {} ms", BetterInfo::timeInMs() - now);
}

void BetterInfoCache::thirdStageLoading() {
    auto now = BetterInfo::timeInMs();
    log::debug("Starting 3rd stage loading BetterInfoCache");

    auto GLM = GameLevelManager::sharedState();
    checkLevelsFromDict(GLM->m_onlineLevels);
    checkLevelsFromDict(GLM->m_dailyLevels);

    log::debug("Finished 3rd stage loading BetterInfoCache in {} ms", BetterInfo::timeInMs() - now);
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
    validateIsObject("failed-levels-dict");
}

void BetterInfoCache::establishCachedDicts(){
    std::shared_lock guard(m_jsonMutex);

    for(auto [key, value] : m_json["coin-count-dict"]) {
        if(value.isNumber()) m_coinCounts[BetterInfo::stoi(key)] = value.asInt().unwrapOr(0);
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

        Loader::get()->queueInMainThread([this] {
            thirdStageLoading();
        });
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
    auto object = matjson::makeObject({
        { "username", std::string(score->m_userName) },
        { "icon-type", (int) score->m_iconType },
        { "iconID", score->m_iconID },
        { "color-1", score->m_color1 },
        { "color-2", score->m_color2 },
        { "special", score->m_special },
        { "accountID", score->m_accountID },
        { "cached", std::time(nullptr) }
    });

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
    if(!m_json["user-info-dict"].contains(idString)) return nullptr;

    auto object = m_json["user-info-dict"][idString];
    if(!object.contains("username") || !object.contains("icon-type") || !object.contains("iconID") || !object.contains("color-1") || !object.contains("color-2") || !object.contains("accountID")) return nullptr;

    auto score = GJUserScore::create();
    score->m_userName = object["username"].asString().unwrapOr(GameLevelManager::sharedState()->tryGetUsername(accountID));
    score->m_iconType = (IconType) object["icon-type"].asInt().unwrapOr(0);
    score->m_iconID = object["iconID"].asInt().unwrapOr(1);
    score->m_color1 = object["color-1"].asInt().unwrapOr(0);
    score->m_color2 = object["color-2"].asInt().unwrapOr(0);
    score->m_accountID = object["accountID"].asInt().unwrapOr(0);
    score->m_special = object["special"].asInt().unwrapOr(0);
    return score;
}

void BetterInfoCache::cacheRatedListsFromMegaResponse(const std::string& megaResponse) {
    log::debug("cacheRatedListsFromMegaResponse BEGIN");

    if(megaResponse.empty() || ServerUtils::isGDPS()) return cacheRatedLists();

    auto GLM = GameLevelManager::sharedState();
    log::debug("cacheRatedListsFromMegaResponse after GLM sharedState");

    size_t hashes = std::count(megaResponse.begin(), megaResponse.end(), '#');
    if(hashes < 3) return cacheRatedLists();

    log::debug("cacheRatedListsFromMegaResponse after cacheRatedLists return");

    std::stringstream responseStream(megaResponse);
    std::string levelData;
    std::string userData;

    getline(responseStream, levelData, '#');
    getline(responseStream, userData, '#');

    std::stringstream userStream(userData);
    std::string currentUser;

    log::debug("cacheRatedListsFromMegaResponse before while getline");
    while(getline(userStream, currentUser, '|')) {
        auto info = utils::string::split(currentUser, ":");
        if(info.size() < 3) continue;

        int userID = BetterInfo::stoi(info[0]);
        int accountID = BetterInfo::stoi(info[2]);

        if(userID > 0) GLM->storeUserName(userID, accountID, info[1]);
    }

    log::debug("cacheRatedListsFromMegaResponse after while getline");

    std::stringstream levelStream(levelData);
    std::string currentLevel;

    log::debug("cacheRatedListsFromMegaResponse before second while getline");
    while(getline(levelStream, currentLevel, '|')) {
        log::debug("cacheRatedListsFromMegaResponse in while getline, currentLevel: {}", currentLevel);
        auto level = GJLevelList::create(BetterInfo::responseToDict(currentLevel));
        log::debug("cacheRatedListsFromMegaResponse after level create, listID: {}", level->m_listID);
        GLM->updateSavedLevelList(level);
        log::debug("cacheRatedListsFromMegaResponse after GLM updateSavedLevelList, listID: {}", level->m_listID);
        //cacheListAsync(level);
        log::debug("cacheRatedListsFromMegaResponse after cacheListAsync, listID: {}", level->m_listID);
        m_updatedCachedLists.emplace(level->m_listID, level);
        log::debug("cacheRatedListsFromMegaResponse after emplace, listID: {}", level->m_listID);
    }

    log::debug("Cached rated lists from mega response");

    checkClaimableLists();
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
                    m_updatedCachedLists.emplace(list->m_listID, list);
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
    log::debug("CheckClaimableLists begin");

    std::unordered_set<int> completedLevels;

    auto GLM = GameLevelManager::sharedState();
    for(auto [key, obj] : CCDictionaryExt<gd::string, GJGameLevel*>(GLM->m_onlineLevels)) {
        if(obj->m_normalPercent < 100) continue;
        completedLevels.insert(obj->m_levelID);
    }

    std::thread([this, completedLevels = std::move(completedLevels)] {
        thread::setName("BI Claimable List Checker");

        log::debug("CheckClaimableLists THREAD begin");

        std::unordered_set<int> completedLists;

        std::shared_lock guard(m_jsonMutex);
        for(auto [key, value] : m_json["list-info-dict"]) {
            if(value["diamonds"].asInt().unwrapOr(0) <= 0) continue;
            
            auto listID = BetterInfo::stoi(key);
            auto levelsRes = value["levels"].asArray();
            if(!levelsRes) continue;

            auto levels = levelsRes.unwrap();
            auto levelsToClaim = value["levels-to-claim"].asInt().unwrapOr(0);

            if(levelsToClaim <= 0) continue;

            int completed = 0;
            for(auto level : levels) {
                if(!level.isNumber()) continue;
                if(completedLevels.find(level.asInt().unwrapOr(-1)) != completedLevels.end()) completed++;
            }

            if(completed >= levelsToClaim) {
                completedLists.insert(listID);
            }
        }

        Loader::get()->queueInMainThread([this, completedLists = std::move(completedLists)] {
            log::debug("CheckClaimableLists THREAD queueInMainThread begin");

            for(auto listID : completedLists) {
                log::debug("Checking if can claim reward for list {}", listID);
                auto list = GJLevelList::create();
                list->m_listID = listID;
                log::debug("before has claimed check {}", listID);
                if(GameStatsManager::sharedState()->hasClaimedListReward(list)) continue;
                log::debug("before claimable lists check {}", listID);
                if(m_claimableLists.contains(listID)) continue;

                log::debug("before updatedcachedlists contains check {}", listID);
                if(m_updatedCachedLists.contains(listID)) {
                    m_claimableLists.emplace(listID, m_updatedCachedLists[listID]);
                    m_updatedCachedLists.erase(listID);
                } else {
                    m_claimableLists.emplace(listID, nullptr);
                }
                log::debug("und das ist alles {}", listID);
                log::info("Can claim reward for list {}", listID);
            }

            m_updatedCachedLists.clear();
            downloadClaimableLists();

            log::debug("CheckClaimableLists THREAD queueInMainThread end");
        });

        log::debug("CheckClaimableLists THREAD end");
    }).detach();

    log::debug("CheckClaimableLists end");
}

void BetterInfoCache::cacheList(GJLevelList* list) {
    std::unique_lock guard(m_jsonMutex);
    auto idString = std::to_string(list->m_listID);
    if(!m_json["list-info-dict"][idString].isObject()) m_json["list-info-dict"][idString] = matjson::Value();

    auto name = std::string(list->m_listName);
    for(auto it = name.begin(); it < name.end(); it++) {
        if(*it < 0x20) *it = ' ';
    }

    //this is needed for android, std::vector<int>(list->m_levels); keeps the vector empty
    std::vector<int> levelsVec;
    for(auto level : list->m_levels) {
        levelsVec.push_back(level);
    }

    m_json["list-info-dict"][idString]["name"] = name;
    m_json["list-info-dict"][idString]["levels"] = levelsVec;
    m_json["list-info-dict"][idString]["levels-to-claim"] = list->m_levelsToClaim;
    m_json["list-info-dict"][idString]["diamonds"] = list->m_diamonds;
    guard.unlock();
    doSave();
}

void BetterInfoCache::cacheListAsync(GJLevelList* list) {
    list->retain();
    std::thread([this, list] {
        thread::setName("BI List Cache");

        cacheList(list);

        Loader::get()->queueInMainThread([list] {
            list->release();
        });
    }).detach();
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

    static std::set<int> downloadingLists;
    std::vector<int> toDownload;
    for(auto [listID, _] : m_claimableLists) {
        if(_ != nullptr) continue;
        if(downloadingLists.contains(listID)) continue;

        toDownload.push_back(listID);
        downloadingLists.insert(listID);
    }

    std::thread([this, toDownload = std::move(toDownload)] {
        for(auto listID : toDownload) {
            Loader::get()->queueInMainThread([this, listID] {
                auto searchObj = GJSearchObject::create(SearchType::Search, std::to_string(listID));
                ServerUtils::getLevelLists(
                    searchObj, 
                    [this](auto lists, bool) {
                        if(lists->empty()) return;
                        m_claimableLists[lists->at(0)->m_listID] = lists->at(0);
                        cacheListAsync(lists->at(0));
                        log::debug("Downloaded list {}", lists->at(0)->m_listID);
                    },
                    false
                );
                log::debug("Downloading list {}", listID);
            });

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

    //fallback code path for when cache fails to load any lists in time
    if(lists->count() == 0 && m_claimableLists.size() > 0) {
        searchObj = GJSearchObject::create(SearchType::Search, std::to_string(m_claimableLists.begin()->first));
        searchObj->m_searchMode = 1;
    }

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

        for(auto currentLvl : *levels) {
            auto idString = std::to_string(currentLvl->m_levelID);
            if(objectExists("level-name-dict", idString) && objectExists("coin-count-dict", idString) && objectExists("demon-difficulty-dict", idString) && getLevelName(currentLvl->m_levelID) != "") continue;

            auto levelFromSaved = LevelUtils::getLevelFromSaved(currentLvl->m_levelID);
            if(levelFromSaved != nullptr && std::string(levelFromSaved->m_levelName) != "") cacheLevel(levelFromSaved);
            else {
                if(isLevelFailed(currentLvl->m_levelID)) continue;
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

void BetterInfoCache::markLevelAsFailed(int id) {
    auto idString = std::to_string(id);

    //std::cout <<  ("Locking unique_lock cacheLevel") << std::endl;
    std::unique_lock guard(m_jsonMutex);
    m_json["failed-levels-dict"][idString] = m_json["failed-levels-dict"][idString].asInt().unwrapOr(0) + 1;
    //std::cout <<  ("Unlocking unique_lock cacheLevel") << std::endl;
}

bool BetterInfoCache::isLevelFailed(int id) {
    auto idString = std::to_string(id);
    if(!objectExists("failed-levels-dict", idString)) return false;

    std::shared_lock guard(m_jsonMutex);
    return m_json["failed-levels-dict"][idString].asInt().unwrapOr(0) >= 5;
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
                auto levelsVector = utils::string::split(searchObj->m_searchQuery, ",");
                std::unordered_set<int> levelsSet;

                for(auto level : levelsVector) {
                    levelsSet.insert(BetterInfo::stoi(level));
                }

                ServerUtils::getOnlineLevels(
                    searchObj, 
                    [this, levelsSet = std::move(levelsSet)](auto levels, bool success, bool explicitError) mutable {
                        std::thread([this, levels, levelsSet = std::move(levelsSet), success, explicitError]() mutable {
                            thread::setName("Level Cache (Inner)");
                            for(const auto& level : *levels) {
                                cacheLevel(level);
                                levelsSet.erase(level->m_levelID.value());
                            }
                            if(success || explicitError) for(const auto& id : levelsSet) {
                                markLevelAsFailed(id);
                            }
                            doSave();
                            Loader::get()->queueInMainThread([levels, failed = levelsSet.size()] {
                                if(levels->size() > 0) log::debug("Cached {} levels; Failed to cache {}", levels->size(), failed);
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

    std::shared_lock guard(m_jsonMutex);
    return m_json["level-name-dict"][idString].asString().unwrapOr("Unknown (malformed JSON)");
}

int BetterInfoCache::getDemonDifficulty(int levelID) {
    auto idString = std::to_string(levelID);
    if(!objectExists("demon-difficulty-dict", idString)) return 0;

    std::shared_lock guard(m_jsonMutex);
    return m_json["demon-difficulty-dict"][idString].asInt().unwrapOr(0);
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
            auto levelBrowserLayer = CCDirector::sharedDirector()->getRunningScene()->getChildByType<LevelBrowserLayer>(0);
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
    if(!m_json["level-info-dict"][idString].isObject()) m_json["level-info-dict"][idString] = matjson::Value();

    m_json["level-info-dict"][idString][field] = value;
    guard.unlock();
    //std::cout <<  ("Unlocking unique_lock storeLevelInfo") << std::endl;
    doSave();
}

std::string BetterInfoCache::getLevelInfo(int levelID, const std::string& field) {
    std::shared_lock guard(m_jsonMutex);
    
    auto idString = std::to_string(levelID);
    return m_json["level-info-dict"][idString][field].asString().unwrapOr("");
}

void BetterInfoCache::storeDatesForLevel(GJGameLevel* level) {
    std::string uploadDateStd = level->m_uploadDate;
    std::string updateDateStd = level->m_updateDate;

    if(!uploadDateStd.empty()) storeLevelInfo(level->m_levelID, "upload-date", uploadDateStd);
    if(!updateDateStd.empty()) storeLevelInfo(level->m_levelID, "update-date", updateDateStd);
}

std::string BetterInfoCache::getUserName(int userID, bool download) {
    if(userID == 0) return ""; //this prevents the request from being sent on every account comments load

    auto idString = std::to_string(userID);
    if(!objectExists("username-dict", idString)) {
        if(download && m_attemptedUsernames.find(userID) == m_attemptedUsernames.end()) {
            std::thread([userID, this] {
                thread::setName("BI Username Downloader");

                async::spawn(
                    ServerUtils::getBaseRequest().get(fmt::format("https://history.geometrydash.eu/api/v1/user/{}/brief/", userID)),
                    [userID, this](web::WebResponse response) {
                        auto json = response.json();
                        if(!response.ok() || json.isErr()) {
                            log::error("Error while getting username for {}: {} - {}", userID, response.code(), response.string().unwrapOr("No response"));
                            return;                            
                        }

                        auto data = json.unwrap();
                        std::string username;

                        username = data["non_player_username"].asString().unwrapOr(data["username"].asString().unwrapOr(""));

                        storeUserName(userID, username);
                        log::debug("Restored green username for {}: {}", userID, username);
                        return;
                    }
                );
            }).detach();
            m_attemptedUsernames.insert(userID);
        }
        return "";
    }

    std::shared_lock guard(m_jsonMutex);
    return m_json["username-dict"][idString].asString().unwrapOr("");
}

int BetterInfoCache::getCoinCount(int levelID) {
    auto it = m_coinCounts.find(levelID);
    if (it != m_coinCounts.end()) return it->second;
    
    std::shared_lock guard(m_jsonMutex);
    auto idString = std::to_string(levelID);
    if(!objectExists("coin-count-dict", idString)) return 3;

    if(auto res = m_json["coin-count-dict"][idString].asInt()) {
        return m_coinCounts[levelID] = res.unwrap();
    }

    return 3;
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
            async::spawn(
                ServerUtils::getBaseRequest().get(fmt::format("https://history.geometrydash.eu/api/v1/date/level/{}/", levelID)),
                [levelID, this](web::WebResponse response) {
                    auto json = response.json();
                    if(!response.ok() || json.isErr()) {
                        log::error("Error while getting exact upload date for level {}: {} - {}", levelID, response.code(), response.string().unwrapOr("No response"));
                        return;
                    }

                    auto data = json.unwrap();
                    if(!data["approx"].isObject()) return;
                    if(!data["approx"]["estimation"].isString()) return;

                    auto res = data["approx"]["estimation"].asString();
                    if(!res) return;

                    storeUploadDate(levelID, res.unwrap());
                }
            );

            m_attemptedLevelDates.insert(levelID);
        }
        return "";
    }

    std::shared_lock guard(m_jsonMutex);
    return m_json["upload-date-dict"][idString].asString().unwrapOr("");
}

void BetterInfoCache::cacheVaultCode(const std::string& id, const std::string& code) {
    if(id.empty() || code.empty()) {
        return;
    }

    m_vaultCodes[id] = code;
}

std::string BetterInfoCache::getVaultCode(const std::string& id) {
    if(m_vaultCodes.find(id) != m_vaultCodes.end()) return m_vaultCodes.at(id);
    return "";
}