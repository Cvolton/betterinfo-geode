#include "CustomCreatorLayer.h"
#include "LevelIDLayer.h"
#include "LevelSearchViewLayer.h"
#include "LevelCategorySearchAlert.h"
#include "../utils.hpp"

CustomCreatorLayer* CustomCreatorLayer::create() {
    auto ret = new CustomCreatorLayer();
    if (ret && ret->init()) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool CustomCreatorLayer::init() {
    auto backgroundSprite = CCSprite::create("game_bg_14_001.png");
    
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto size = backgroundSprite->getContentSize();
    
    backgroundSprite->setScale(winSize.width / size.width);
    
    backgroundSprite->setAnchorPoint({0, 0});
    backgroundSprite->setPosition({0,-75});
    
    backgroundSprite->setColor({164, 0, 255});
    
    backgroundSprite->setZOrder(-2);
    addChild(backgroundSprite);

    auto backBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png"),
        this,
        menu_selector(CustomCreatorLayer::onBack)
    );

    auto menuBack = CCMenu::create();
    menuBack->addChild(backBtn);
    menuBack->setPosition({25, winSize.height - 25});

    addChild(menuBack);

    setKeypadEnabled(true);

    auto menu = CCMenu::create();
    menu->setID("bi-main-menu");
    addChild(menu);
    auto label = CCLabelBMFont::create(Mod::get()->getName().c_str(), "goldFont.fnt");

    label->setPosition({winSize.width / 2, winSize.height - 25});

    addChild(label);

    auto featuredBtn = CCMenuItemSpriteExtra::create(
        BetterInfo::createWithBISpriteFrameName("BI_featuredBtn_001.png"),
        this,
        menu_selector(CustomCreatorLayer::onFeatured)
    );
    menu->addChild(featuredBtn);
    featuredBtn->setPosition({-120,55});
    featuredBtn->setSizeMult(1.2f);
    featuredBtn->setID("bi-featured-button");

    auto mostLikedBtn = CCMenuItemSpriteExtra::create(
        BetterInfo::createWithBISpriteFrameName("BI_mostLikedBtn_001.png"),
        this,
        menu_selector(CustomCreatorLayer::onMostLiked)
    );
    menu->addChild(mostLikedBtn);
    mostLikedBtn->setPosition({0,55});
    mostLikedBtn->setSizeMult(1.2f);
    mostLikedBtn->setID("bi-most-liked-button");

    auto searchBtn = CCMenuItemSpriteExtra::create(
        BetterInfo::createBISprite("BI_searchLevel_001.png"),
        this,
        menu_selector(CustomCreatorLayer::onSearch)
    );
    menu->addChild(searchBtn);
    searchBtn->setPosition({120,55});
    searchBtn->setSizeMult(1.2f);
    searchBtn->setID("bi-search-button");

    auto searchIDBtn = CCMenuItemSpriteExtra::create(
        BetterInfo::createBISprite("BI_searchID_001.png"),
        this,
        menu_selector(CustomCreatorLayer::onSearchID)
    );
    menu->addChild(searchIDBtn);
    searchIDBtn->setPosition({120,-57.5f});
    searchIDBtn->setSizeMult(1.2f);
    searchIDBtn->setID("bi-search-id-button");

    auto dailyBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_dailyBtn_001.png"),
        this,
        menu_selector(CustomCreatorLayer::onDaily)
    );
    menu->addChild(dailyBtn);
    dailyBtn->setPosition({-122.5,-55});
    dailyBtn->setSizeMult(1.2f);
    dailyBtn->setID("bi-daily-button");

    //CCTextureCache::sharedTextureCache()->addImage("mostliked.png", 0);

    auto weeklyBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_weeklyBtn_001.png"),
        this,
        menu_selector(CustomCreatorLayer::onWeekly)
    );
    menu->addChild(weeklyBtn);
    weeklyBtn->setPosition({-2.5,-55});
    weeklyBtn->setSizeMult(1.2f);
    weeklyBtn->setID("bi-weekly-button");

    auto settingsSprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    settingsSprite->setScale(0.775f);
    auto settingsBtn = CCMenuItemSpriteExtra::create(
        settingsSprite,
        this,
        menu_selector(CustomCreatorLayer::onSettings)
    );
    menu->addChild(settingsBtn);
    settingsBtn->setPosition({0, - ( (winSize.height / 2) - 25 ) });
    settingsBtn->setSizeMult(1.2f);
    settingsBtn->setID("bi-settings-button");

    auto infoBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"),
        this,
        menu_selector(CustomCreatorLayer::onInfo)
    );
    menu->addChild(infoBtn);
    infoBtn->setPosition({ (winSize.width / 2) - 25 , (winSize.height / 2) - 25 });
    infoBtn->setSizeMult(1.2f);
    infoBtn->setID("bi-info-button");

    auto cornerBL = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    cornerBL->setPosition({0,0});
    cornerBL->setAnchorPoint({0,0});
    addChild(cornerBL, -1);
    cornerBL->setID("bi-corner-bl");

    auto cornerUL = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    cornerUL->setPosition({0,winSize.height});
    cornerUL->setAnchorPoint({0,0});
    cornerUL->setRotation(90);
    cornerUL->setID("bi-corner-ul");
    addChild(cornerUL, -1);

    auto cornerUR = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    cornerUR->setPosition({winSize.width,winSize.height});
    cornerUR->setAnchorPoint({0,0});
    cornerUR->setRotation(180);
    cornerUR->setID("bi-corner-ur");
    addChild(cornerUR, -1);

    auto cornerBR = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    cornerBR->setPosition({winSize.width,0});
    cornerBR->setAnchorPoint({0,0});
    cornerBR->setRotation(270);
    cornerBR->setID("bi-corner-br");
    addChild(cornerBR, -1);

    return true;
}

void CustomCreatorLayer::keyBackClicked() {
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}


void CustomCreatorLayer::onBack(CCObject* object) {
    keyBackClicked();
}

void CustomCreatorLayer::onInfo(CCObject* object) {
    FLAlertLayer::create(
        nullptr, 
        Mod::get()->getName().c_str(), 
        "This is the main menu for all features related to the mod.\n\n<cy>Featured:</c> Levels featured in Geometry Dash World\n<cg>Most liked:</c> \"Most Liked\" in Geometry Dash World,\nreal purpose unknown.\n<cj>Search (ID):</c> View comments of any level ID.\n\n<cl>Settings:</c> Opens the mod settings\n<cr>Update:</c> Opens the mod update menu",
        "OK", 
        nullptr,
        450
    )->show();
}

void CustomCreatorLayer::onSearch(CCObject* object) {
    LevelCategorySearchAlert::create()->show();
}

void CustomCreatorLayer::onSearchID(CCObject* object) {
    LevelIDLayer::create()->show();
}

void CustomCreatorLayer::onFeatured(CCObject* object) {
    auto searchObject = GJSearchObject::create(SearchType::FeaturedGDW);
    auto browserLayer = LevelBrowserLayer::scene(searchObject);

    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);

    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void CustomCreatorLayer::onMostLiked(CCObject* object) {
    auto searchObject = GJSearchObject::create(SearchType::LikedGDW);
    auto browserLayer = LevelBrowserLayer::scene(searchObject);

    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);

    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void CustomCreatorLayer::onDaily(CCObject* object) {
    auto searchObject = GJSearchObject::create(SearchType::DailyVault);
    auto browserLayer = LevelBrowserLayer::scene(searchObject);

    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);

    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void CustomCreatorLayer::onWeekly(CCObject* object) {
    auto searchObject = GJSearchObject::create(SearchType::WeeklyVault);
    auto browserLayer = LevelBrowserLayer::scene(searchObject);

    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);

    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void CustomCreatorLayer::onSettings(CCObject* object) {
    FLAlertLayer::create(Mod::get()->getName().c_str(), "Use the Geode settings to adjust settings related to the mod.", "OK")->show();
}

CCScene* CustomCreatorLayer::scene() {
    auto layer = CustomCreatorLayer::create();
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}