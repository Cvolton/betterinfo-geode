#include "ServerUtils.h"
#include "../utils.hpp"

#include <Geode/utils/web.hpp>

using namespace geode::prelude;

std::string ServerUtils::getBaseURL() {
    // The addresses are pointing to "https://www.boomlings.com/database/getGJLevels21.php"
    // in the main game executable
    char* originalUrl = nullptr;
    #ifdef GEODE_IS_WINDOWS
        static_assert(GEODE_COMP_GD_VERSION == 22040, "Unsupported GD version");
        originalUrl = (char*)(base::get() + 0x410a74); //not a mistake
    #elif defined(GEODE_IS_MACOS)
        static_assert(GEODE_COMP_GD_VERSION == 22000, "Unsupported GD version");
        originalUrl = (char*)(base::get() + 0x83c079);
    #elif defined(GEODE_IS_ANDROID64)
        static_assert(GEODE_COMP_GD_VERSION == 22050, "Unsupported GD version");
        originalUrl = (char*)(base::get() + 0xE861D0);
    #elif defined(GEODE_IS_ANDROID32)
        static_assert(GEODE_COMP_GD_VERSION == 22050, "Unsupported GD version");
        originalUrl = (char*)(base::get() + 0x93E3F0);
    #else
        static_assert(false, "Unsupported platform");
    #endif

    std::string ret = originalUrl;
    if(ret.size() > 34) ret = ret.substr(0, 34);

    return ret;
}

void ServerUtils::getOnlineLevels(GJSearchObject* searchObject, std::function<void(std::vector<Ref<GJGameLevel>>, bool)> callback) {
    std::string completedLevels = ""; //TODO: get completed levels

    std::string postString = fmt::format("{}&type={}&str={}&diff={}&len={}&page={}&total={}&uncompleted={}&onlyCompleted={}&featured={}&original={}&twoPlayer={}&coins={}",
        GameLevelManager::sharedState()->getBasePostString(), (int) searchObject->m_searchType, searchObject->m_searchQuery, searchObject->m_difficulty, searchObject->m_length, searchObject->m_page,
        searchObject->m_total, searchObject->m_uncompletedFilter ? 1 : 0, searchObject->m_completedFilter ? 1 : 0, searchObject->m_featuredFilter ? 1 : 0, searchObject->m_originalFilter ? 1 : 0,
        searchObject->m_twoPlayerFilter ? 1 : 0, searchObject->m_coinsFilter ? 1 : 0);

    if(searchObject->m_epicFilter) postString += "&epic=1";
    if(searchObject->m_legendaryFilter) postString += "&legendary=1";
    if(searchObject->m_mythicFilter) postString += "&mythic=1";
    if(searchObject->m_searchType == SearchType::UsersLevels) postString += "&local=1";
    if(searchObject->m_songFilter) {
        postString += fmt::format("&song={}", searchObject->m_songID);
        if(searchObject->m_customSongFilter) postString += "&customSong=1";
    }
    if(searchObject->m_uncompletedFilter || searchObject->m_completedFilter) postString += fmt::format("&completedLevels={}", completedLevels);
    if(searchObject->m_searchType == SearchType::LevelListsOnClick) postString += "&inc=1";

    postString += "&secret=Wmfd2893gb7";

    std::string key = searchObject->getKey();

    web::AsyncWebRequest()
        .userAgent("")
        .postRequest()
        .bodyRaw(postString)
        .fetch(fmt::format("{}/getGJLevels21.php", getBaseURL()))
        .text()
        .then([callback, key](web::SentAsyncWebRequest& request, const std::string& response) {
            size_t hashes = std::count(response.begin(), response.end(), '#');
            if(hashes < 4) return callback({}, false);

            std::stringstream responseStream(response);
            std::string levelData;
            std::string userData;
            std::string songData;
            std::string pageData;
            std::vector<Ref<GJGameLevel>> levels;

            getline(responseStream, levelData, '#');
            getline(responseStream, userData, '#');
            getline(responseStream, songData, '#');
            getline(responseStream, pageData, '#');

            std::stringstream userStream(userData);
            std::string currentUser;
            while(getline(userStream, currentUser, '|')) {
                auto info = utils::string::split(currentUser, ":");

                int userID = std::stoi(info[0]);
                int accountID = std::stoi(info[2]);

                if(userID > 0) GameLevelManager::sharedState()->storeUserName(userID, accountID, info[1]);
            }

            std::stringstream levelStream(levelData);
            std::string currentLevel;
            while(getline(levelStream, currentLevel, '|')) {
                auto level = GJGameLevel::create(BetterInfo::responseToDict(currentLevel), false);
                levels.push_back(level);
            }

            CCArray* levelArray = CCArray::create();
            for(auto level : levels) levelArray->addObject(level);

            GameLevelManager::sharedState()->saveFetchedLevels(levelArray);
            if(key.length() < 255) GameLevelManager::sharedState()->storeSearchResult(levelArray, pageData, key.c_str());

            callback(levels, true);
        })
        .expect([callback](const std::string& response) {
            //getting headers is currently not supported, gotta wait for new index...

            callback({}, false);
        });
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
        Notification::create(message, NotificationIcon::Error, 5.f)->show();
        return true;
    }
    return false;
}