#include "LevelCategorySearchAlert.h"
#include "LevelSearchViewLayer.h"
#include "../../utils.hpp"

LevelCategorySearchAlert* LevelCategorySearchAlert::create(){
    auto ret = new LevelCategorySearchAlert();
    if (ret && ret->init()) {
        //robert 1 :D
        ret->autorelease();
    } else {
        //robert -1
        delete ret;
        ret = nullptr;
    }
    return ret;
}

void LevelCategorySearchAlert::onClose(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void LevelCategorySearchAlert::onPlayed(CCObject* sender) {
    auto browserLayer = LevelSearchViewLayer::scene(LevelUtils::completedDeque());
    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);
    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void LevelCategorySearchAlert::onFavorites(cocos2d::CCObject* sender) {
    BISearchObject searchObj;
    searchObj.favorite = true;

    auto browserLayer = LevelSearchViewLayer::scene(LevelUtils::completedDeque(), searchObj);
    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);
    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void LevelCategorySearchAlert::onFolder(cocos2d::CCObject* sender) {
    auto popup = SetFolderPopup::create(0, false, "Go to Folder");
    popup->m_delegate = this;
    popup->m_scene = this;
    popup->show();
}

void LevelCategorySearchAlert::onCompleted(cocos2d::CCObject* sender) {
    BISearchObject searchObj;
    searchObj.completed = true;

    auto browserLayer = LevelSearchViewLayer::scene(LevelUtils::completedDeque(), searchObj);
    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);
    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void LevelCategorySearchAlert::onOrbs(cocos2d::CCObject* sender) {
    BISearchObject searchObj;
    searchObj.completedOrbs = true;

    auto browserLayer = LevelSearchViewLayer::scene(LevelUtils::completedDeque(), searchObj);
    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);
    CCDirector::sharedDirector()->pushScene(transitionFade);
}
void LevelCategorySearchAlert::onLeaderboard(cocos2d::CCObject* sender) {
    BISearchObject searchObj;
    searchObj.completedLeaderboard = true;

    auto browserLayer = LevelSearchViewLayer::scene(LevelUtils::completedDeque(), searchObj);
    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);
    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void LevelCategorySearchAlert::onCoins(cocos2d::CCObject* sender) {
    BISearchObject searchObj;
    searchObj.completedCoins = true;
    searchObj.completed = true;

    auto browserLayer = LevelSearchViewLayer::scene(LevelUtils::completedDeque(), searchObj);
    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);
    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void LevelCategorySearchAlert::onNoCoins(cocos2d::CCObject* sender) {
    BISearchObject searchObj;
    searchObj.uncompletedCoins = true;
    searchObj.completed = true;

    auto browserLayer = LevelSearchViewLayer::scene(LevelUtils::completedDeque(), searchObj);
    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);
    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void LevelCategorySearchAlert::setIDPopupClosed(SetIDPopup* popup, int value) {
    BISearchObject searchObj;
    searchObj.folder = value;

    auto browserLayer = LevelSearchViewLayer::scene(LevelUtils::completedDeque(), searchObj);
    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);
    CCDirector::sharedDirector()->pushScene(transitionFade);
}

bool LevelCategorySearchAlert::init(){
    bool init = cocos2d::CCLayerColor::initWithColor({0x00, 0x00, 0x00, 0x4B});
    if(!init) return false;

    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    //director->getTouchDispatcher()->incrementForcePrio(2);

    setTouchEnabled(true);
    setKeypadEnabled(true);

    cocos2d::CCSize winSize = director->getWinSize();
    m_mainLayer = cocos2d::CCLayer::create();

    this->addChild(m_mainLayer);

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png", { 0.0f, 0.0f, 80.0f, 80.0f });
    bg->setContentSize({ 360.0f, 190.0f });
    m_mainLayer->addChild(bg, -1);
    bg->setPosition({ winSize.width / 2, winSize.height / 2 });
    bg->setID("background-square"_spr);

    auto closeButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png"),
        this,
        menu_selector(LevelCategorySearchAlert::onClose)
    );
    closeButton->setID("close-button"_spr);

    m_buttonMenu = CCMenu::create();
    m_buttonMenu->setID("button-menu"_spr);
    m_mainLayer->addChild(m_buttonMenu, 10);
    m_buttonMenu->addChild(closeButton);
    closeButton->setPosition({-170.5f, 79});
    closeButton->setSizeMult(1.2f);

    auto searchTitle = CCLabelBMFont::create("Search", "bigFont.fnt");
    searchTitle->setPosition({0,71});
    searchTitle->setID("title-label"_spr);

    m_buttonMenu->addChild(searchTitle);

    auto separator = CCSprite::createWithSpriteFrameName("floorLine_001.png");
    separator->setPosition({285,207});
    separator->setScaleX(0.75f);
    separator->setOpacity(100);
    separator->setID("separator"_spr);
    m_mainLayer->addChild(separator);

    //auto playedButton = createButton(m_buttonMenu, "Played", menu_selector(LevelCategorySearchAlert::onPlayed), -57, 18, (int)(120*0.6), 44*0.6f, 0.6f);
    auto playedButton = BetterInfo::createSearchButton(this, "Played", "BI_playIcon_001.png"_spr, menu_selector(LevelCategorySearchAlert::onPlayed), .5f, .35f);
    playedButton->setPosition({-117, 18});
    playedButton->setID("played-button"_spr);
    m_buttonMenu->addChild(playedButton);

    //auto favoriteButton = createButton(m_buttonMenu, "Favorites", menu_selector(LevelCategorySearchAlert::onFavorites), 57, 18, (int)(120*0.6), 44*0.6f, 0.6f);
    auto favoriteButton = BetterInfo::createSearchButton(this, "Favorites", "gj_heartOn_001.png", menu_selector(LevelCategorySearchAlert::onFavorites), .45f, .6f);
    favoriteButton->setPosition({0, 18});
    favoriteButton->setID("favorite-button"_spr);
    m_buttonMenu->addChild(favoriteButton);

    auto folderButton = BetterInfo::createSearchButton(this, "Folder", "folderIcon_001.png", menu_selector(LevelCategorySearchAlert::onFolder), .45f, .525f);
    folderButton->setPosition({117, 18});
    folderButton->setID("folder-button"_spr);
    auto folderSearchBtn = static_cast<SearchButton*>(folderButton->getNormalImage());
    folderSearchBtn->m_icon->setPositionX(folderSearchBtn->m_icon->getPositionX() + 1);
    folderSearchBtn->m_icon->setPositionY(folderSearchBtn->m_icon->getPositionY() + 1);
    m_buttonMenu->addChild(folderButton);

    //auto completedButton = createButton(m_buttonMenu, "Completed", menu_selector(LevelCategorySearchAlert::onCompleted), -114, -22, (int)(120*0.6), 44*0.6f, 0.6f);
    auto completedButton = BetterInfo::createSearchButton(this, "Completed", "GJ_completesIcon_001.png", menu_selector(LevelCategorySearchAlert::onCompleted), .45f, .5f);
    completedButton->setPosition({-117, -22});
    completedButton->setID("completed-button"_spr);
    auto completedSearchBtn = static_cast<SearchButton*>(completedButton->getNormalImage());
    completedSearchBtn->m_icon->setPositionX(completedSearchBtn->m_icon->getPositionX() - 3); // 48
    completedSearchBtn->m_label->setPositionX(completedSearchBtn->m_label->getPositionX() + 1); // 98
    m_buttonMenu->addChild(completedButton);

    //auto orbsButton = createButton(m_buttonMenu, "C. With Orbs", menu_selector(LevelCategorySearchAlert::onOrbs), 0, -22, (int)(120*0.6), 44*0.6f, 0.6f);
    auto orbsButton = BetterInfo::createSearchButton(this, "C. With Orbs", "currencyOrbIcon_001.png", menu_selector(LevelCategorySearchAlert::onOrbs), .35f, .75f);
    orbsButton->setPosition({0, -22});
    orbsButton->setID("orbs-button"_spr);
    auto orbsSearchBtn = static_cast<SearchButton*>(orbsButton->getNormalImage());
    orbsSearchBtn->m_icon->setPositionX(orbsSearchBtn->m_icon->getPositionX() - 1.5);
    orbsSearchBtn->m_label->setPositionX(orbsSearchBtn->m_label->getPositionX() + 1.5);
    m_buttonMenu->addChild(orbsButton);

    //auto leaderboardButton = createButton(m_buttonMenu, "C. Leaderboard", menu_selector(LevelCategorySearchAlert::onLeaderboard), 114, -22, (int)(120*0.6), 44*0.6f, 0.6f);
    auto leaderboardButton = BetterInfo::createSearchButton(this, "C. Leaderboard", "rankIcon_top10_001.png", menu_selector(LevelCategorySearchAlert::onLeaderboard), .3f, .6f);
    leaderboardButton->setPosition({117, -22});
    leaderboardButton->setID("leaderboard-button"_spr);
    auto leaderboardSearchBtn = static_cast<SearchButton*>(leaderboardButton->getNormalImage());
    leaderboardSearchBtn->m_icon->setPositionX(leaderboardSearchBtn->m_icon->getPositionX() - 1);
    leaderboardSearchBtn->m_label->setPositionX(leaderboardSearchBtn->m_label->getPositionX() + 1);
    m_buttonMenu->addChild(leaderboardButton);

    //auto coinsButton = createButton(m_buttonMenu, "C. With Coins", menu_selector(LevelCategorySearchAlert::onCoins), -57, -64, (int)(120*0.6), 44*0.6f, 0.6f);
    auto coinsButton = BetterInfo::createSearchButton(this, "C. With Coins", "usercoin_small01_001.png", menu_selector(LevelCategorySearchAlert::onCoins), .325f, 1.1f);
    coinsButton->setPosition({-58.5, -64});
    coinsButton->setID("coins-button"_spr);
    auto coinsSearchBtn = static_cast<SearchButton*>(coinsButton->getNormalImage());
    coinsSearchBtn->m_icon->setPositionX(coinsSearchBtn->m_icon->getPositionX() - 1);
    coinsSearchBtn->m_label->setPositionX(coinsSearchBtn->m_label->getPositionX() + 2);
    m_buttonMenu->addChild(coinsButton);

    //auto noCoinsButton = createButton(m_buttonMenu, "C. Without Coins", menu_selector(LevelCategorySearchAlert::onNoCoins), 57, -64, (int)(120*0.6), 44*0.6f, 0.6f);
    auto noCoinsButton = BetterInfo::createSearchButton(this, "C. W/o Coins", "usercoin_small01_001.png", menu_selector(LevelCategorySearchAlert::onNoCoins), .35f, 1.1f);
    noCoinsButton->setPosition({58.5, -64});
    noCoinsButton->setID("no-coins-button"_spr);
    static_cast<SearchButton*>(noCoinsButton->getNormalImage())->m_icon->setColor({165, 165, 165});
    auto noCoinsSearchBtn = static_cast<SearchButton*>(noCoinsButton->getNormalImage());
    noCoinsSearchBtn->m_icon->setPositionX(noCoinsSearchBtn->m_icon->getPositionX() - 1);
    noCoinsSearchBtn->m_label->setPositionX(noCoinsSearchBtn->m_label->getPositionX() + 1);
    m_buttonMenu->addChild(noCoinsButton);

    return true;
}

CCMenuItemSpriteExtra* LevelCategorySearchAlert::createButton(CCNode* menu, const char* text, cocos2d::SEL_MenuHandler handler, float x, float y, int width, float height, float scale){
    auto buttonSprite = ButtonSprite::create(text, width, true, "bigFont.fnt", "GJ_button_01.png", height, scale);
    auto buttonButton = CCMenuItemSpriteExtra::create(
        buttonSprite,
        this,
        handler
    );
    buttonButton->setSizeMult(1.2f);
    buttonButton->setPosition({x,y});
    menu->addChild(buttonButton);

    return buttonButton;
}