#include "CustomCreatorLayer.h"
#include "LevelIDLayer.h"
#include "LevelFiltering/LevelSearchViewLayer.h"
#include "LevelFiltering/LevelCategorySearchAlert.h"
#include "../utils.hpp"
#include <Geode/ui/GeodeUI.hpp>

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
    if (!BIBaseLayer::init()) return false;
    
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto menu = CCMenu::create();
    menu->setID("main-menu"_spr);
    addChild(menu);
    auto label = CCLabelBMFont::create(Mod::get()->getName().c_str(), "goldFont.fnt");

    label->setPosition({winSize.width / 2, winSize.height - 25});
    label->setID("title-label"_spr);

    addChild(label);

    auto featuredBtn = CCMenuItemSpriteExtra::create(
        BetterInfo::createWithBISpriteFrameName("BI_featuredBtn_001.png"),
        this,
        menu_selector(CustomCreatorLayer::onFeatured)
    );
    menu->addChild(featuredBtn);
    featuredBtn->setPosition({-60,55});
    featuredBtn->setSizeMult(1.2f);
    featuredBtn->setID("featured-button"_spr);

    /*auto mostLikedBtn = CCMenuItemSpriteExtra::create(
        BetterInfo::createWithBISpriteFrameName("BI_mostLikedBtn_001.png"),
        this,
        menu_selector(CustomCreatorLayer::onMostLiked)
    );
    menu->addChild(mostLikedBtn);
    mostLikedBtn->setPosition({0,55});
    mostLikedBtn->setSizeMult(1.2f);
    mostLikedBtn->setID("most-liked-button"_spr);*/

    auto searchBtn = CCMenuItemSpriteExtra::create(
        BetterInfo::createBISprite("BI_searchLevel_001.png"),
        this,
        menu_selector(CustomCreatorLayer::onSearch)
    );
    menu->addChild(searchBtn);
    searchBtn->setPosition({60,-55});
    searchBtn->setSizeMult(1.2f);
    searchBtn->setID("search-button"_spr);

    auto searchIDBtn = CCMenuItemSpriteExtra::create(
        BetterInfo::createBISprite("BI_searchID_001.png"),
        this,
        menu_selector(CustomCreatorLayer::onSearchID)
    );
    menu->addChild(searchIDBtn);
    searchIDBtn->setPosition({-60,-55});
    searchIDBtn->setSizeMult(1.2f);
    searchIDBtn->setID("search-id-button"_spr);

    /*auto dailyBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_dailyBtn_001.png"),
        this,
        menu_selector(CustomCreatorLayer::onDaily)
    );
    menu->addChild(dailyBtn);
    dailyBtn->setPosition({-122.5,-55});
    dailyBtn->setSizeMult(1.2f);
    dailyBtn->setID("daily-button"_spr);

    //CCTextureCache::sharedTextureCache()->addImage("mostliked.png", 0);

    auto weeklyBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_weeklyBtn_001.png"),
        this,
        menu_selector(CustomCreatorLayer::onWeekly)
    );
    menu->addChild(weeklyBtn);
    weeklyBtn->setPosition({-2.5,-55});
    weeklyBtn->setSizeMult(1.2f);
    weeklyBtn->setID("weekly-button"_spr);*/

    auto fameBtn = CCMenuItemSpriteExtra::create(
        BetterInfo::createBISprite("GJ_fameBtn_001.png"),
        this,
        menu_selector(CustomCreatorLayer::onFame)
    );
    menu->addChild(fameBtn);
    fameBtn->setPosition({60,55});
    fameBtn->setSizeMult(1.2f);
    fameBtn->setID("weekly-button"_spr);

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
    settingsBtn->setID("settings-button"_spr);

    auto infoBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"),
        this,
        menu_selector(CustomCreatorLayer::onInfo)
    );
    menu->addChild(infoBtn);
    infoBtn->setPosition({ (winSize.width / 2) - 25 , (winSize.height / 2) - 25 });
    infoBtn->setSizeMult(1.2f);
    infoBtn->setID("info-button"_spr);

    return true;
}

void CustomCreatorLayer::onInfo(CCObject* object) {
    FLAlertLayer::create(
        nullptr, 
        Mod::get()->getName().c_str(), 
            "This is the main menu for all features related to the mod.\n"
            "\n"
            "<cy>Featured:</c> Levels featured in Geometry Dash World\n"
            "<cr>Hall of Fame:</c> Epic levels (as seen in GD 2.1)\n"
            "<cg>Search (ID):</c> View comments of any level ID.\n"
            "<cj>Level Search:</c> Show a list of levels based on your progress\n"
            "\n"
            "<cp>Settings:</c> Opens the mod settings",
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
    auto searchObject = GJSearchObject::create(SearchType::DailySafe);
    auto browserLayer = LevelBrowserLayer::scene(searchObject);

    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);

    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void CustomCreatorLayer::onWeekly(CCObject* object) {
    auto searchObject = GJSearchObject::create(SearchType::WeeklySafe);
    auto browserLayer = LevelBrowserLayer::scene(searchObject);

    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);

    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void CustomCreatorLayer::onFame(CCObject* object) {
    auto searchObject = GJSearchObject::create(SearchType::HallOfFame);
    auto browserLayer = LevelBrowserLayer::scene(searchObject);

    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);

    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void CustomCreatorLayer::onSettings(CCObject* object) {
    //open settings menu instead of the alert :)
    openSettingsPopup(Mod::get());
}

CCScene* CustomCreatorLayer::scene() {
    auto layer = CustomCreatorLayer::create();
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}
