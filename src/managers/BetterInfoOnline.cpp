#include "BetterInfoOnline.h"
#include "../utils.hpp"

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

    #ifndef GEODE_IS_MACOS
    //only on forced reload or if we dont have cached
    CCHttpRequest* request = new CCHttpRequest;
    request->setUrl("http://www.boomlings.com/database/getGJScores20.php");
    request->setRequestType(CCHttpRequest::HttpRequestType::kHttpPost);
    request->setResponseCallback(this, httpresponse_selector(BetterInfoOnline::onScoresFinished));
    //TODO: make this slightly more dynamic
    auto postData = CCString::createWithFormat("gameVersion=21&binaryVersion=35&gdw=0&accountID=%i&type=relative&secret=Wmfd2893gb7", accountID);
    request->setRequestData(
        postData->getCString(), strlen(postData->getCString())
    );
    auto ccInt = CCInteger::create(accountID);
    ccInt->retain();
    request->setUserData(ccInt);
    CCHttpClient::getInstance()->send(request);
    //request->release();
    #endif
}

void BetterInfoOnline::onScoresFinished(CCHttpClient* client, CCHttpResponse* response){
    if(!(response->isSucceed())) return;

    gd::vector<char>* responseData = response->getResponseData();
    std::string responseString(responseData->begin(), responseData->end());

    auto ccInt = (CCInteger*) (response->getHttpRequest()->getUserData());
    int accountID = ccInt->getValue();
    ccInt->release();
    generateScores(responseString, accountID);

    sendScores(m_scoreDict[accountID], accountID);
}

void BetterInfoOnline::generateScores(const std::string& response, int accountID){

    if(m_scoreDict.contains(accountID)) { 
        m_scoreDict[accountID]->release();
        m_scoreDict[accountID] = nullptr;
    }

    CCArray* scores = CCArray::create();
    scores->retain();
    m_scoreDict[accountID] = scores;

    if(response == "-1") return;

    std::stringstream responseStream(response);
    std::string current;

    while(getline(responseStream, current, '|')){
        
        //auto CM = CvoltonManager::sharedState();
        auto GLM = GameLevelManager::sharedState();

        auto score = GJUserScore::create(
            //CM->responseToDict(current)
            //GLM->responseToDict(current, false)
            BetterInfo::responseToDict(current)
        );

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
                case IconType::DeathEffect:
                case IconType::Special:
                    break;
            }

            m_scoreProfilePage->userInfoChanged(score);
            m_scoreProfilePage = nullptr;
            break;
        }
    }
}