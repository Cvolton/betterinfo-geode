#include "BetterInfoOnline.h"
#include "BetterInfoCache.h"
#include "../utils.hpp"
#include <Geode/utils/web.hpp>
#include <Geode/cocos/support/base64.h>

bool BetterInfoOnline::init(){
    return true;
}

BetterInfoOnline::BetterInfoOnline(){}


void BetterInfoOnline::loadScores(int accountID, bool force, BILeaderboardDelegate* delegate) {
    loadScores(accountID, force, delegate, nullptr);
}

void BetterInfoOnline::loadScores(int accountID, bool force, Ref<ProfilePage> profilePage) {
    loadScores(accountID, force, nullptr, profilePage);
}

void BetterInfoOnline::loadScores(int accountID, bool force, BILeaderboardDelegate* delegate, Ref<ProfilePage> profilePage){
    //cache optimization
    if(!force && m_scoreDict.contains(accountID)) {
        sendScores(m_scoreDict[accountID], accountID, delegate, profilePage);
        return;
    }

    static std::unordered_map<int, web::WebTask> tasks;
    tasks.emplace(accountID, ServerUtils::getBaseRequest(false)
        .bodyString(fmt::format("{}&accountID={}&udid={}&type=relative&secret=Wmfd2893gb7", ServerUtils::getBasePostString(false), accountID, std::string(GameManager::sharedState()->m_playerUDID)))
        .post(fmt::format("{}/getGJScores20.php", ServerUtils::getBaseURL()))
        .map([this, accountID, delegate, profilePage](web::WebResponse* response) {
            if(response->ok()) {
                generateScores(response->string().unwrapOr(""), accountID);
                sendScores(m_scoreDict[accountID], accountID, delegate, profilePage);
                BetterInfoCache::sharedState()->cacheScoresResult(m_scoreDict[accountID]);
            }

            return *response;
        })
    );
}

void BetterInfoOnline::generateScores(const std::string& response, int accountID){
    auto GM = GameManager::sharedState();
    auto BICache = BetterInfoCache::sharedState();

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

void BetterInfoOnline::sendScores(cocos2d::CCArray* scores, int accountID, BILeaderboardDelegate* delegate, Ref<ProfilePage> profilePage){
    if(delegate) {
        delegate->onLeaderboardFinished(scores);
        delegate = nullptr;
    }else if(profilePage) {
        sendScoreToProfilePage(scores, accountID, profilePage);
    }
}

void BetterInfoOnline::sendScoreToProfilePage(cocos2d::CCArray* scores, int accountID, Ref<ProfilePage> profilePage){
    if(!profilePage) return;

    CCObject* obj;
    CCARRAY_FOREACH(scores, obj){
        auto score = static_cast<GJUserScore*>(obj);
        score->m_messageState = 2;
        score->m_friendStatus = 1;
        score->m_glowEnabled = score->m_special == 2;
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

            profilePage->userInfoChanged(score);
            break;
        }
    }
}
