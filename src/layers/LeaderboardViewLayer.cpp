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

    //refresh btn
    auto refreshBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png"),
        this,
        menu_selector(LeaderboardViewLayer::onRefresh)
    );
    refreshBtn->setID("refresh-button"_spr);

    auto menuRefresh = CCMenu::create();
    menuRefresh->addChild(refreshBtn);
    menuRefresh->setPosition({winSize.width - 26.75f, 26.75f});
    menuRefresh->setZOrder(2);
    menuRefresh->setID("refresh-menu"_spr);

    this->addChild(menuRefresh);

    setData(CCArray::create());

    loadPage();

    std::array<std::pair<const char*, const char*>, 4> stats = {
        std::make_pair("GJ_starsIcon_001.png", "stars"),
        std::make_pair("GJ_moonsIcon_001.png", "moons"),
        std::make_pair("GJ_demonIcon_001.png", "demons"),
        std::make_pair("GJ_coinsIcon2_001.png", "user-coins")
    };
    auto rightMenu = CCMenu::create();
    rightMenu->setID("right-side-menu"_spr);
    rightMenu->setContentSize({ 24.f, 150.f });
    rightMenu->setPosition({ winSize.width / 2 + 220.f, winSize.height / 2 });
    rightMenu->setLayout(
        ColumnLayout::create()
            ->setGap(11.f)
            ->setAxisAlignment(AxisAlignment::Center)
            ->setAxisReverse(true)
            ->setAutoScale(false)
    );

    int i = 0;
    for (const auto& stat : stats) {
        auto btn = CCMenuItemExt::createSpriteExtra(
            ButtonSprite::create(CCSprite::createWithSpriteFrameName(stat.first)),
            [this, i] (auto sprite) {
                this->loadStat(i);
            }
        );
        btn->setID(fmt::format("{}-button"_spr, stat.second));
        rightMenu->addChild(btn);
        i++;
    }

    rightMenu->updateLayout();
    this->addChild(rightMenu);

    this->onRefresh(nullptr);

    return true;
}

void LeaderboardViewLayer::loadPage(){
    if(!m_data) return;

    m_listView = CustomListView::create(m_data, BoomListType::Score, 220.f, 356.f);
    BIViewLayer::loadPage();
}

void LeaderboardViewLayer::keyBackClicked() {
    BIViewLayer::keyBackClicked();
}

void LeaderboardViewLayer::onRefresh(CCObject* object) {
    this->loadStat(m_stat); 
}

void LeaderboardViewLayer::loadStat(int stat) {
    BetterInfoOnline::sharedState()->loadScores(m_accountID, true, this, nullptr, stat);

    showCircle();
    m_stat = stat;
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

LeaderboardViewLayer::~LeaderboardViewLayer() {
    BetterInfoOnline::sharedState()->cancelScoresRequest(this);
}