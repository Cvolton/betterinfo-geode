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

    m_rightMenu = CCMenu::create();
    m_rightMenu->setID("right-side-menu"_spr);
    m_rightMenu->setContentSize({ 24.f, 150.f });
    m_rightMenu->setPosition({ winSize.width / 2 + 220.f, winSize.height / 2 });
    m_rightMenu->setLayout(
        ColumnLayout::create()
            ->setGap(11.f)
            ->setAxisAlignment(AxisAlignment::Center)
            ->setAxisReverse(true)
            ->setAutoScale(false)
    );

    this->setupStatBtns();
    this->addChild(m_rightMenu);

    this->loadStat(0, false);

    return true;
}

void LeaderboardViewLayer::setupStatBtns() {
    for(const auto& btn : m_statBtns) {
        btn->removeFromParent();
    }
    m_statBtns.clear();

    std::array<std::pair<const char*, const char*>, 4> stats = {
        std::make_pair("GJ_starsIcon_001.png", "stars"),
        std::make_pair("GJ_moonsIcon_001.png", "moons"),
        std::make_pair("GJ_demonIcon_001.png", "demons"),
        std::make_pair("GJ_coinsIcon2_001.png", "user-coins")
    };

    int i = 0;
    for (const auto& stat : stats) {
        auto icon = CCSprite::createWithSpriteFrameName(stat.first);
        auto sprite = ButtonSprite::create(icon, 32, 0, 320.0, 1.0, true, m_stat == i ? "GJ_button_02.png" : "GJ_button_01.png", false);
        sprite->updateSpriteOffset({0, -1.5f});
        sprite->setScale(0.6f);
        icon->setScale(1.2f);
        auto btn = CCMenuItemExt::createSpriteExtra(
            sprite,
            [this, i] (auto sprite) {
                this->loadStat(i, false);
            }
        );
        btn->setID(fmt::format("{}-button"_spr, stat.second));
        btn->setSizeMult(1.5f);
        m_rightMenu->addChild(btn);
        m_statBtns.push_back(btn);
        i++;
    }

    m_rightMenu->updateLayout();
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
    this->loadStat(m_stat, true); 
}

void LeaderboardViewLayer::loadStat(int stat, bool reload) {
    setData(CCArray::create());
    loadPage();

    showCircle();
    m_stat = stat;

    setupStatBtns();

    BetterInfoOnline::sharedState()->loadScores(m_accountID, reload, this, nullptr, stat);
}

CCScene* LeaderboardViewLayer::scene(int accountID) {
    auto layer = LeaderboardViewLayer::create(accountID);
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}

void LeaderboardViewLayer::onLeaderboardFinished(cocos2d::CCArray* scores, int stat) {
    if(stat != m_stat) return;

    setData(scores);
    loadPage();
    hideCircle();
}

LeaderboardViewLayer::~LeaderboardViewLayer() {
    BetterInfoOnline::sharedState()->cancelScoresRequest(this);
}