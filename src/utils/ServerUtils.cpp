#include "ServerUtils.h"
#include "../utils.hpp"

#include <Geode/utils/web.hpp>

using namespace geode::prelude;

const char* ServerUtils::getBaseURL() {
    return "http://www.boomlings.com/database";
}

void ServerUtils::getOnlineLevels(GJSearchObject* searchObject, std::function<void(std::vector<GJGameLevel*>, bool)> callback) {
    //TODO: if key is valid, cache levels
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

    web::AsyncWebRequest()
        .userAgent("")
        .postRequest()
        .bodyRaw(postString)
        .fetch(fmt::format("{}/getGJLevels21.php", getBaseURL()))
        .text()
        .then([callback](const std::string& response) {
            size_t hashes = std::count(response.begin(), response.end(), '#');
            if(hashes < 4) return callback({}, false);

            std::stringstream responseStream(response);
            std::string levelData;
            std::string userData;
            std::vector<GJGameLevel*> levels;

            getline(responseStream, levelData, '#');
            getline(responseStream, userData, '#');

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

            callback(levels, true);
        });
}