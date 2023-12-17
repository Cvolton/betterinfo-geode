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

    showCircle();

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

    setData(CCArray::create());

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

    BIViewLayer::keyBackClicked();
}

void LeaderboardViewLayer::onRefresh(CCObject* object) {
    BetterInfoOnline::sharedState()->loadScores(m_accountID, true, this);

    showCircle();
}

CCScene* LeaderboardViewLayer::scene(int accountID) {
    auto layer = LeaderboardViewLayer::create(accountID);
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}

void LeaderboardViewLayer::onLeaderboardFinished(cocos2d::CCArray* scores) {
    setData(scores);
    loadPage();
    hideCircle();
}