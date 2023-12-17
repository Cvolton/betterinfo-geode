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
    BIViewLayer::init(false);

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    m_accountID = accountID;
    m_title = "Global Leaderboards";

    m_data = CCArray::create();
    m_data->retain();

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
    if(!m_data) return;

    m_listView = CustomListView::create(m_data, BoomListType::Score, 220.f, 356.f);
    BIViewLayer::loadPage();
}

void LeaderboardViewLayer::keyBackClicked() {
    BetterInfoOnline::sharedState()->m_scoreDelegate = nullptr;
    
    if(m_circle) m_circle->release();
    m_circle = nullptr;

    BIViewLayer::keyBackClicked();
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
    m_data = scores;
    m_data->retain();
    loadPage();
    if(m_circle) m_circle->fadeAndRemove();
}