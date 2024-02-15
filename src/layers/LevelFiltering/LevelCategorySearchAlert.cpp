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

    /*auto neighborButton = createButton(m_buttonMenu, "Neighbors", menu_selector(LevelCategorySearchAlert::onNeighbors), 0, 4, (int)(120*0.6), 44*0.6f, 0.6f);
    auto similarButton = createButton(m_buttonMenu, "Similar", menu_selector(LevelCategorySearchAlert::onSimilar), 0, -48, (int)(120*0.6), 44*0.6f, 0.6f);*/

    auto similarButton = createButton(m_buttonMenu, "Played", menu_selector(LevelCategorySearchAlert::onPlayed), -57, 18, (int)(120*0.6), 44*0.6f, 0.6f);
    similarButton->setID("similar-button"_spr);
    auto favoriteButton = createButton(m_buttonMenu, "Favorites", menu_selector(LevelCategorySearchAlert::onFavorites), 57, 18, (int)(120*0.6), 44*0.6f, 0.6f);
    favoriteButton->setID("favorite-button"_spr);
    auto completedButton = createButton(m_buttonMenu, "Completed", menu_selector(LevelCategorySearchAlert::onCompleted), -114, -22, (int)(120*0.6), 44*0.6f, 0.6f);
    completedButton->setID("completed-button"_spr);
    auto orbsButton = createButton(m_buttonMenu, "C. With Orbs", menu_selector(LevelCategorySearchAlert::onOrbs), 0, -22, (int)(120*0.6), 44*0.6f, 0.6f);
    orbsButton->setID("orbs-button"_spr);
    auto leaderboardButton = createButton(m_buttonMenu, "C. Leaderboard", menu_selector(LevelCategorySearchAlert::onLeaderboard), 114, -22, (int)(120*0.6), 44*0.6f, 0.6f);
    leaderboardButton->setID("leaderboard-button"_spr);
    auto coinsButton = createButton(m_buttonMenu, "C. With Coins", menu_selector(LevelCategorySearchAlert::onCoins), -57, -64, (int)(120*0.6), 44*0.6f, 0.6f);
    coinsButton->setID("coins-button"_spr);
    auto noCoinsButton = createButton(m_buttonMenu, "C. Without Coins", menu_selector(LevelCategorySearchAlert::onNoCoins), 57, -64, (int)(120*0.6), 44*0.6f, 0.6f);
    noCoinsButton->setID("no-coins-button"_spr);
    /*auto songButton = createButton(m_buttonMenu, "Leaderboard", menu_selector(LevelCategorySearchAlert::onLeaderboard), 0, -48, (int)(120*0.6), 44*0.6f, 0.6f);*/

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