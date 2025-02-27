#include "ServerUtils.h"
#include "../utils.hpp"
#include "../integrations/ServerAPIEvents.hpp"

#include <shared_mutex>

using namespace geode::prelude;

static inline std::unordered_map<std::string, Ref<cocos2d::CCArray>> s_cache;
static inline std::unordered_map<std::string, web::WebTask> s_requests;

static inline std::shared_mutex s_requestsMutex;
static bool s_isGDPS = false;

bool ServerUtils::isGDPS() {
    return getBaseURL() != "https://www.boomlings.com/database";
}

web::WebRequest ServerUtils::getBaseRequest(bool setUserAgent) {
    return web::WebRequest().userAgent(setUserAgent ? fmt::format("BetterInfo {} / Geode {}", Mod::get()->getVersion().toVString(true), Loader::get()->getVersion().toVString(true)) : "");
}

std::string ServerUtils::getBaseURL() {
    if(Loader::get()->isModLoaded("km7dev.server_api")) {
        auto url = ServerAPIEvents::getCurrentServer().url;
        if(!url.empty() && url != "NONE_REGISTERED") {
            while(url.ends_with("/")) url.pop_back();
            return url;
        }
    }

    // The addresses are pointing to "https://www.boomlings.com/database/getGJLevels21.php"
    // in the main game executable
    char* originalUrl = nullptr;
    #ifdef GEODE_IS_WINDOWS
        static_assert(GEODE_COMP_GD_VERSION == 22074, "Unsupported GD version");
        originalUrl = (char*)(base::get() + 0x53ea48);
    #elif defined(GEODE_IS_ARM_MAC)
        static_assert(GEODE_COMP_GD_VERSION == 22074, "Unsupported GD version");
        originalUrl = (char*)(base::get() + 0x7749fb);
    #elif defined(GEODE_IS_INTEL_MAC)
        static_assert(GEODE_COMP_GD_VERSION == 22074, "Unsupported GD version");
        originalUrl = (char*)(base::get() + 0x8516bf);
    #elif defined(GEODE_IS_ANDROID64)
        static_assert(GEODE_COMP_GD_VERSION == 22074, "Unsupported GD version");
        originalUrl = (char*)(base::get() + 0xEA2988);
    #elif defined(GEODE_IS_ANDROID32)
        static_assert(GEODE_COMP_GD_VERSION == 22074, "Unsupported GD version");
        originalUrl = (char*)(base::get() + 0x952E9E);
    #else
        static_assert(false, "Unsupported platform");
    #endif

    std::string ret = originalUrl;
    if(ret.size() > 34) ret = ret.substr(0, 34);

    return ret;
}

std::string ServerUtils::getBasePostString(bool includeAccount) {
    auto ret =  fmt::format("gameVersion={}&binaryVersion={}&gdw={}", 22, 42, 0);

    if(includeAccount) {
        auto GJAM = GJAccountManager::sharedState();
        auto GM = GameManager::sharedState();
        ret += fmt::format("&udid={}&uuid={}", GM->m_playerUDID, GM->m_playerUserID.value());
        if(GJAM->m_accountID > 0) ret += fmt::format("&accountID={}&gjp2={}", GJAM->m_accountID, GJAM->m_GJP2);
    }


    return ret;
}

std::string ServerUtils::getSearchObjectKey(GJSearchObject* searchObject) {
    if(searchObject->m_searchType == SearchType::Users) return fmt::format("{}_{}_{}", (int) SearchType::Users, searchObject->m_searchQuery, searchObject->m_page);

    std::array<std::string, 3> stringKeys = {searchObject->m_searchQuery, searchObject->m_difficulty, searchObject->m_length};
    std::array<int, 18> intKeys = {
        searchObject->m_page,
        searchObject->m_starFilter,
        searchObject->m_uncompletedFilter,
        searchObject->m_featuredFilter,
        searchObject->m_songID,
        searchObject->m_originalFilter,
        searchObject->m_twoPlayerFilter,
        searchObject->m_customSongFilter,
        searchObject->m_songFilter,
        searchObject->m_noStarFilter,
        searchObject->m_coinsFilter,
        searchObject->m_epicFilter,
        searchObject->m_legendaryFilter,
        searchObject->m_mythicFilter,
        searchObject->m_completedFilter,
        (int) searchObject->m_demonFilter,
        searchObject->m_folder,
        searchObject->m_searchMode
    };

    std::string ret = std::to_string((int) searchObject->m_searchType) + "_";

    for(auto key : stringKeys) ret += (key + "_");
    for(auto key : intKeys) ret += (std::to_string(key) + "_");

    ret.pop_back();
    return ret;
}

void ServerUtils::getLevelLists(GJSearchObject* searchObject, std::function<void(std::shared_ptr<std::vector<Ref<GJLevelList>>>, bool)> callback, bool cacheLevels) {
    std::string completedLevels = "";

    std::string postString = fmt::format("{}&str={}&type={}&page={}",
        getBasePostString(), searchObject->m_searchQuery, (int) searchObject->m_searchType, searchObject->m_page);

    // TODO: in theory this is not fully thread safe
    // - it can crash if you follow someone while its iterating the followed list
    // however i cannot think of a way to reproduce this in-game without doing it
    // in code, so i will leave fixing this for later
    postString += GameLevelManager::sharedState()->writeSpecialFilters(searchObject);

    postString += "&secret=Wmfd2893gb7";

    std::string key = getSearchObjectKey(searchObject);
    auto requestKey = fmt::format("getLevelLists_{}", key);

    std::unique_lock lock(s_requestsMutex);
    if(s_requests.find(requestKey) == s_requests.end()) {
        s_requests.emplace(requestKey, getBaseRequest(false).bodyString(postString).post(fmt::format("{}/getGJLevelLists.php", getBaseURL())));
    }

    auto task = s_requests[requestKey].map([callback, key, requestKey, cacheLevels](web::WebResponse* response) {
        auto GLM = GameLevelManager::sharedState();

        std::unique_lock lock(s_requestsMutex);
        s_requests.erase(requestKey);
        lock.unlock();

        if(!response->ok()) {
            if(response->header("retry-after").has_value()) {
                showRateLimitError(BetterInfo::stoi(response->header("retry-after").value()));
            } else {
                showCFError(response->string().unwrapOr(""));
            }

            auto levels = std::make_shared<std::vector<Ref<GJLevelList>>>();

            callback(levels, false);

            return *response;
        }

        auto levels = std::make_shared<std::vector<Ref<GJLevelList>>>();

        auto responseString = response->string().unwrapOr("");

        size_t hashes = std::count(responseString.begin(), responseString.end(), '#');
        if(hashes < 3) {
            callback(levels, false);
            return *response;
        }

        std::stringstream responseStream(responseString);
        std::string levelData;
        std::string userData;
        std::string pageData;

        getline(responseStream, levelData, '#');
        getline(responseStream, userData, '#');
        getline(responseStream, pageData, '#');

        std::stringstream userStream(userData);
        std::string currentUser;
        while(getline(userStream, currentUser, '|')) {
            auto info = utils::string::split(currentUser, ":");
            if(info.size() < 3) continue;

            int userID = BetterInfo::stoi(info[0]);
            int accountID = BetterInfo::stoi(info[2]);

            if(userID > 0) GLM->storeUserName(userID, accountID, info[1]);
        }

        std::stringstream levelStream(levelData);
        std::string currentLevel;
        while(getline(levelStream, currentLevel, '|')) {
            auto level = GJLevelList::create(BetterInfo::responseToDict(currentLevel));
            levels->push_back(level);
        }

        CCArray* levelArray = CCArray::create();
        for(auto level : *levels) {
            levelArray->addObject(level);
            GLM->updateSavedLevelList(level);
        }

        if(cacheLevels) {
            if(key.length() < 255) GLM->storeSearchResult(levelArray, pageData, key.c_str());
            else s_cache[key] = levelArray;
        }

        callback(levels, true);

        return *response;
    });

    s_requests.erase(requestKey);
    s_requests.emplace(requestKey, task);
}

void ServerUtils::getOnlineLevels(GJSearchObject* searchObject, std::function<void(std::shared_ptr<std::vector<Ref<GJGameLevel>>>, bool success, bool explicitError)> callback, bool cacheLevels) {
    std::string completedLevels = "";

    std::string postString = fmt::format("{}&type={}&str={}&diff={}&len={}&page={}&total={}&uncompleted={}&onlyCompleted={}&featured={}&original={}&twoPlayer={}&coins={}",
        getBasePostString(), (int) searchObject->m_searchType, searchObject->m_searchQuery, searchObject->m_difficulty, searchObject->m_length, searchObject->m_page,
        searchObject->m_total, searchObject->m_uncompletedFilter ? 1 : 0, searchObject->m_completedFilter ? 1 : 0, searchObject->m_featuredFilter ? 1 : 0, searchObject->m_originalFilter ? 1 : 0,
        searchObject->m_twoPlayerFilter ? 1 : 0, searchObject->m_coinsFilter ? 1 : 0);

    if(searchObject->m_epicFilter) postString += "&epic=1";
    if(searchObject->m_legendaryFilter) postString += "&legendary=1";
    if(searchObject->m_mythicFilter) postString += "&mythic=1";
    if(searchObject->m_searchType == SearchType::UsersLevels) postString += fmt::format("&local={}", BetterInfo::stoi(searchObject->m_searchQuery) == GameManager::sharedState()->m_playerUserID);
    if(searchObject->m_songFilter) {
        postString += fmt::format("&song={}", searchObject->m_songID);
        if(searchObject->m_customSongFilter) postString += "&customSong=1";
    }
    if(searchObject->m_uncompletedFilter || searchObject->m_completedFilter) {
        auto completedArray = CCArrayExt<GJGameLevel*>(GameLevelManager::sharedState()->getCompletedLevels(GameManager::sharedState()->getGameVariable("0073")));
        bool first = true;

        for(auto level : completedArray) {
            if(!first) completedLevels += ",";
            completedLevels += fmt::format("{}", level->m_levelID.value());
            first = false;
        }

        postString += fmt::format("&completedLevels={}", completedLevels);
    }
    //if(searchObject->m_searchType == SearchType::LevelListsOnClick) postString += "&inc=1";

    // TODO: in theory this is not fully thread safe
    // - it can crash if you follow someone while its iterating the followed list
    // however i cannot think of a way to reproduce this in-game without doing it
    // in code, so i will leave fixing this for later
    postString += GameLevelManager::sharedState()->writeSpecialFilters(searchObject);

    postString += "&secret=Wmfd2893gb7";

    std::string key = getSearchObjectKey(searchObject);
    auto requestKey = fmt::format("getOnlineLevels_{}", key);

    std::unique_lock lock(s_requestsMutex);
    if(s_requests.find(requestKey) == s_requests.end()) {
        s_requests.emplace(requestKey, getBaseRequest(false).bodyString(postString).post(fmt::format("{}/getGJLevels21.php", getBaseURL())));
    }

    auto task = s_requests[requestKey].map([callback, key, requestKey, cacheLevels](web::WebResponse* response) {
        std::unique_lock lock(s_requestsMutex);
        s_requests.erase(requestKey);
        lock.unlock();

        if(!response->ok()) {
            if(response->header("retry-after").has_value()) {
                showRateLimitError(BetterInfo::stoi(response->header("retry-after").value()));
            } else {
                showCFError(response->string().unwrapOr(""));
            }

            auto levels = std::make_shared<std::vector<Ref<GJGameLevel>>>();

            callback(levels, false, false);

            return *response;
        }

        auto levels = std::make_shared<std::vector<Ref<GJGameLevel>>>();

        auto responseString = response->string().unwrapOr("");

        if(responseString == "-1") {
            callback(levels, false, true);
            return *response;
        }

        size_t hashes = std::count(responseString.begin(), responseString.end(), '#');
        if(hashes < 4) {
            callback(levels, false, false);
            return *response;
        }

        std::stringstream responseStream(responseString);
        std::string levelData;
        std::string userData;
        std::string songData;
        std::string pageData;

        getline(responseStream, levelData, '#');
        getline(responseStream, userData, '#');
        getline(responseStream, songData, '#');
        getline(responseStream, pageData, '#');

        MusicDownloadManager::sharedState()->createSongsInfo(songData, "");

        std::stringstream userStream(userData);
        std::string currentUser;
        while(getline(userStream, currentUser, '|')) {
            auto info = utils::string::split(currentUser, ":");
            if(info.size() < 3) continue;

            int userID = BetterInfo::stoi(info[0]);
            int accountID = BetterInfo::stoi(info[2]);

            if(userID > 0) GameLevelManager::sharedState()->storeUserName(userID, accountID, info[1]);
        }

        std::stringstream levelStream(levelData);
        std::string currentLevel;
        while(getline(levelStream, currentLevel, '|')) {
            auto level = GJGameLevel::create(BetterInfo::responseToDict(currentLevel), false);
            levels->push_back(level);
        }

        CCArray* levelArray = CCArray::create();
        for(auto level : *levels) levelArray->addObject(level);

        GameLevelManager::sharedState()->saveFetchedLevels(levelArray);
        if(cacheLevels) {
            if(key.length() < 255) GameLevelManager::sharedState()->storeSearchResult(levelArray, pageData, key.c_str());
            else s_cache[key] = levelArray;
        }

        callback(levels, true, false);

        return *response;
    });

    s_requests.erase(requestKey);
    s_requests.emplace(requestKey, task);
}

CCArray* ServerUtils::getStoredOnlineLevels(const std::string& key) {
    if(key.length() < 255) return GameLevelManager::sharedState()->getStoredOnlineLevels(key.c_str());

    if(s_cache.find(key) != s_cache.end()) return s_cache[key];
    return nullptr;
}

void ServerUtils::resetCache() {
    s_cache.clear();
}

bool ServerUtils::showCFError(const std::string& data) {
    if(data.starts_with("error code:")) {
        auto error = BetterInfo::stoi(data.substr(12));
        std::string message = "";
        switch(error) {
            case 1005:
                message = "  Your ISP is banned from GD.\n  If you are using a VPN, try disabling it.\n  (error code: 1005)";
                break;
            case 1006:
                message = "  Your IP address is banned from GD.\n  If you are using a VPN, try disabling it.\n  (error code: 1006)";
                break;
            case 1015:
                message = " Rate limited by RobTop's server.\n (error code: 1015)";
                break;
            case 1020:
                message = fmt::format(" Server is down for maintenance.\n (error code: {})", error);
                break;
            default:
                message = fmt::format("  A server error has occurred.\n  (error code: {})", error);
                break;
        }
        BetterInfo::showUnimportantNotification(message, NotificationIcon::Error, 5.f);
        return true;
    }
    return false;
}

bool ServerUtils::showRateLimitError(int seconds) {
    BetterInfo::showUnimportantNotification(fmt::format(" Rate limited by RobTop's server\n Try again in {}", GameToolbox::getTimeString(seconds, false)), NotificationIcon::Warning, 5.f);
    return true;
}