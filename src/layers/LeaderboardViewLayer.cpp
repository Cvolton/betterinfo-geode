#include "LeaderboardViewLayer.h"
#include "JumpToPageLayer.h"
//#include "../managers/CvoltonManager.h"
#include "../managers/BetterInfoOnline.h"
#include "../utils.hpp"

LeaderboardViewLayer* LeaderboardViewLayer::create(int accountID) {
    auto ret = new LeaderboardViewLayer();
    if (ret && ret->init(accountID)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool LeaderboardViewLayer::init(int accountID) {

    /*auto CM = CvoltonManager::sharedState();
    CM->loadTextures();
    CM->setOption("has_viewed_as", true);*/
    m_accountID = accountID;

    auto GLM = GameLevelManager::sharedState();
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto backgroundSprite = CCSprite::create("game_bg_14_001.png");
    bool controllerConnected = BetterInfo::controllerConnected();

    backgroundSprite->setScale(winSize.width / backgroundSprite->getContentSize().width);
    backgroundSprite->setAnchorPoint({0, 0});
    backgroundSprite->setPosition({0,-75});
    backgroundSprite->setColor({164, 0, 255}); //purple
    backgroundSprite->setZOrder(-2);
    addChild(backgroundSprite);

    auto menu = CCMenu::create();
    addChild(menu);
    
    auto backBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
        this,
        menu_selector(LeaderboardViewLayer::onBack)
    );

    backBtn->setPosition({(-winSize.width / 2) + 25, (winSize.height / 2) - 25});
    menu->addChild(backBtn);

    /*auto menuBack = CCMenu::create();
    menuBack->addChild(backBtn);
    menuBack->setPosition({25, winSize.height - 25});

    addChild(menuBack);*/

    setTouchEnabled(true);
    setKeypadEnabled(true);

    m_scores = CCArray::create();
    m_scores->retain();

    //corners
    auto cornerBL = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    cornerBL->setPosition({0,0});
    cornerBL->setAnchorPoint({0,0});
    addChild(cornerBL, -1);

    auto cornerBR = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    cornerBR->setPosition({winSize.width,0});
    cornerBR->setAnchorPoint({0,0});
    cornerBR->setRotation(270);
    addChild(cornerBR, -1);

    m_circle = LoadingCircle::create();
    m_circle->retain();
    m_circle->setParentLayer(this);
    m_circle->show();

    //refresh btn
    auto refreshBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png"),
        this,
        menu_selector(LeaderboardViewLayer::onRefresh)
    );

    auto menuRefresh = CCMenu::create();
    menuRefresh->addChild(refreshBtn);
    menuRefresh->setPosition({winSize.width - 26.75f, 26.75f});
    menuRefresh->setZOrder(2);
    menuRefresh->setID("bi-refresh-menu");

    this->addChild(menuRefresh);

    loadPage();
    BetterInfoOnline::sharedState()->loadScores(m_accountID, false, this);

    return true;
}

void LeaderboardViewLayer::loadPage(){
    if(!m_scores) return;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    if(m_listLayer != nullptr) m_listLayer->removeFromParentAndCleanup(true);

    if(m_leaderboardView) m_leaderboardView->release();
    m_leaderboardView = CustomListView::create(m_scores, BoomListType::Score, 220.f, 356.f);
    m_listLayer = GJListLayer::create(m_leaderboardView, "Global Leaderboards", {191, 114, 62, 255}, 356.f, 220.f);
    m_listLayer->setPosition(winSize / 2 - m_listLayer->getScaledContentSize() / 2 - CCPoint(0,5));
    addChild(m_listLayer);
}

void LeaderboardViewLayer::keyBackClicked() {
    BetterInfoOnline::sharedState()->m_scoreDelegate = nullptr;
    
    setTouchEnabled(false);
    setKeypadEnabled(false);

    if(m_scores) m_scores->release();
    if(m_circle) m_circle->release();
    m_scores = nullptr;
    m_circle = nullptr;

    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}

void LeaderboardViewLayer::onBack(CCObject* object) {
    keyBackClicked();
}

void LeaderboardViewLayer::onRefresh(CCObject* object) {
    BetterInfoOnline::sharedState()->loadScores(m_accountID, true, this);

    if(m_circle) {
        m_circle->fadeAndRemove();
        m_circle->release();
    }

    m_circle = LoadingCircle::create();
    m_circle->retain();
    m_circle->setParentLayer(this);
    m_circle->show();
}

CCScene* LeaderboardViewLayer::scene(int accountID) {
    auto layer = LeaderboardViewLayer::create(accountID);
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}

void LeaderboardViewLayer::onLeaderboardFinished(cocos2d::CCArray* scores) {
    m_scores = scores;
    m_scores->retain();
    loadPage();
    if(m_circle) m_circle->fadeAndRemove();
}