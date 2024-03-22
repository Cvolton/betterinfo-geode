#include "BetterInfoOnline.h"
#include "../utils.hpp"
#include <Geode/utils/web.hpp>
#include <Geode/cocos/support/base64.h>

bool BetterInfoOnline::init(){
    return true;
}

BetterInfoOnline::BetterInfoOnline(){}


void BetterInfoOnline::loadScores(int accountID, bool force, BILeaderboardDelegate* delegate) {
    m_scoreDelegate = delegate;
    loadScores(accountID, force);
}

void BetterInfoOnline::loadScores(int accountID, bool force, ProfilePage* profilePage) {
    m_scoreProfilePage = profilePage;
    loadScores(accountID, force);
}

void BetterInfoOnline::loadScores(int accountID, bool force){
    //cache optimization
    if(!force && m_scoreDict.contains(accountID)) {
        sendScores(m_scoreDict[accountID], accountID);
        return;
    }

    web::AsyncWebRequest()
        .userAgent("")
        .postRequest()
        .bodyRaw(fmt::format("gameVersion=21&binaryVersion=35&gdw=0&accountID={}&udid={}&type=relative&secret=Wmfd2893gb7", accountID, std::string(GameManager::sharedState()->m_playerUDID)))
        .fetch(fmt::format("{}/getGJScores20.php", ServerUtils::getBaseURL()))
        .text()
        .then([this, accountID](const std::string& response) {
            generateScores(response, accountID);
            sendScores(m_scoreDict[accountID], accountID);
        });
}

void BetterInfoOnline::generateScores(const std::string& response, int accountID){
    auto GM = GameManager::sharedState();

    if(m_scoreDict.contains(accountID)) { 
        m_scoreDict[accountID] = nullptr;
    }

    CCArray* scores = CCArray::create();
    m_scoreDict[accountID] = scores;

    if(response == "-1") return;

    std::stringstream responseStream(response);
    std::string current;

    while(getline(responseStream, current, '|')){

        auto score = GJUserScore::create(
            BetterInfo::responseToDict(current)
        );

        GameLevelManager::sharedState()->storeUserName(score->m_userID, score->m_accountID, score->m_userName);

        if(std::string(score->m_userUDID) != "") score->m_userUDID = GM->m_playerUDID;

        scores->addObject(score);
    }
}

void BetterInfoOnline::sendScores(cocos2d::CCArray* scores, int accountID){
    if(m_scoreDelegate) {
        m_scoreDelegate->onLeaderboardFinished(scores);
        m_scoreDelegate = nullptr;
    }else if(m_scoreProfilePage) {
        sendScoreToProfilePage(scores, accountID);
    }
}

void BetterInfoOnline::sendScoreToProfilePage(cocos2d::CCArray* scores, int accountID){
    if(!m_scoreProfilePage) return;

    CCObject* obj;
    CCARRAY_FOREACH(scores, obj){
        auto score = static_cast<GJUserScore*>(obj);
        score->m_messageState = 2;
        score->m_friendStatus = 1;
        if(score->m_accountID == accountID) {
            score->m_globalRank = score->m_playerRank;
            switch(score->m_iconType) {
                case IconType::Cube:
                    score->m_playerCube = score->m_iconID;
                    break;
                case IconType::Ship:
                    score->m_playerShip = score->m_iconID;
                    break;
                case IconType::Ball:
                    score->m_playerBall = score->m_iconID;
                    break;
                case IconType::Ufo:
                    score->m_playerUfo = score->m_iconID;
                    break;
                case IconType::Wave:
                    score->m_playerWave = score->m_iconID;
                    break;
                case IconType::Robot:
                    score->m_playerRobot = score->m_iconID;
                    break;
                case IconType::Spider:
                    score->m_playerSpider = score->m_iconID;
                    break;
                case IconType::Swing:
                    score->m_playerSwing = score->m_iconID;
                    break;
                case IconType::Jetpack:
                    score->m_playerJetpack = score->m_iconID;
                    break;
                default:
                    break;
            }

            m_scoreProfilePage->userInfoChanged(score);
            m_scoreProfilePage = nullptr;
            break;
        }
    }
}
