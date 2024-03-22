#include "CustomLevelSearchLayer.h"

using namespace geode::prelude;

CustomLevelSearchLayer* CustomLevelSearchLayer::create(GJGameLevel* level){
    auto ret = new CustomLevelSearchLayer();
    if (ret && ret->init(level)) {
        //robert 1 :D
        ret->autorelease();
    } else {
        //robert -1
        delete ret;
        ret = nullptr;
    }
    return ret;
}

void CustomLevelSearchLayer::onClose(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void CustomLevelSearchLayer::onSimilar(CCObject* sender) {
    auto searchObject = GJSearchObject::create(SearchType::Similar, std::to_string(m_level->m_levelID));
    auto browserLayer = LevelBrowserLayer::scene(searchObject);

    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);

    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void CustomLevelSearchLayer::onSong(CCObject* sender) {
    bool customSong = m_level->m_songID > 0;
    int songID = customSong ? m_level->m_songID : m_level->m_audioTrack;

    auto searchObject = GJSearchObject::create(SearchType::MostLiked);
    searchObject->m_customSongFilter = customSong;
    searchObject->m_songID = songID;
    searchObject->m_songFilter = true;

    auto browserLayer = LevelBrowserLayer::scene(searchObject);

    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);

    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void CustomLevelSearchLayer::onNeighbors(CCObject* sender) {
    std::ostringstream query;
    bool addSeparator = false;
    for(int i = (m_level->m_levelID) - 50; i < ((m_level->m_levelID) + 50); i++){
        if(addSeparator) query << ",";
        query << i;
        addSeparator = true;
    }

    auto searchObject = GJSearchObject::create(SearchType::Type19, query.str());
    auto browserLayer = LevelBrowserLayer::scene(searchObject);
    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);
    CCDirector::sharedDirector()->pushScene(transitionFade);
}

bool CustomLevelSearchLayer::init(GJGameLevel* level){
    bool init = cocos2d::CCLayerColor::initWithColor({0x00, 0x00, 0x00, 0x4B});
    if(!init) return false;

    this->m_level = level;

    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    /*director->getTouchDispatcher()->incrementForcePrio(2);*/

    setTouchEnabled(true);
    setKeypadEnabled(true);

    cocos2d::CCSize winSize = director->getWinSize();
    m_mainLayer = cocos2d::CCLayer::create();

    this->addChild(m_mainLayer);

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png", { 0.0f, 0.0f, 80.0f, 80.0f });
    bg->setContentSize({ 360.0f, 180.0f });
    m_mainLayer->addChild(bg, -1);
    m_mainLayer->setID("main-layer"_spr);
    bg->setPosition({ winSize.width / 2, winSize.height / 2 });

    auto closeButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png"),
        this,
        menu_selector(CustomLevelSearchLayer::onClose)
    );

    m_buttonMenu = CCMenu::create();
    m_buttonMenu->setID("button-menu"_spr);
    m_mainLayer->addChild(m_buttonMenu, 10);
    m_buttonMenu->addChild(closeButton);
    closeButton->setPosition({-170.5f, 79});
    closeButton->setSizeMult(1.2f);
    closeButton->setID("close-button"_spr);

    auto searchTitle = CCLabelBMFont::create("Search", "bigFont.fnt");
    searchTitle->setPosition({0,66});
    searchTitle->setID("search-title"_spr);

    m_buttonMenu->addChild(searchTitle);

    auto separator = CCSprite::createWithSpriteFrameName("floorLine_001.png");
    separator->setPosition({285,202});
    separator->setScaleX(0.75f);
    separator->setOpacity(100);
    separator->setID("separator"_spr);
    m_mainLayer->addChild(separator);

    /*auto buttonSprite = ButtonSprite::create("Neighbors", (int)(120*0.6), true, "bigFont.fnt", "GJ_button_01.png", 44*0.6f, 0.6f);
    auto buttonButton = CCMenuItemSpriteExtra::create(
        buttonSprite,
        this,
        menu_selector(CustomLevelSearchLayer::onNeighbors)
    );
    buttonButton->setSizeMult(1.2f);
    buttonButton->setPosition({0,0});
    m_buttonMenu->addChild(buttonButton);*/

    /*auto neighborButton = createButton(m_buttonMenu, "Neighbors", menu_selector(CustomLevelSearchLayer::onNeighbors), 0, 4, (int)(120*0.6), 44*0.6f, 0.6f);
    auto similarButton = createButton(m_buttonMenu, "Similar", menu_selector(CustomLevelSearchLayer::onSimilar), 0, -48, (int)(120*0.6), 44*0.6f, 0.6f);*/

    auto similarButton = createButton(m_buttonMenu, "Similar", menu_selector(CustomLevelSearchLayer::onSimilar), -75, 8, (int)(120*0.6), 44*0.6f, 0.6f);
    similarButton->setID("similar-button"_spr);
    auto neighborButton = createButton(m_buttonMenu, "Neighbors", menu_selector(CustomLevelSearchLayer::onNeighbors), 75, 8, (int)(120*0.6), 44*0.6f, 0.6f);
    neighborButton->setID("neighbor-button"_spr);
    auto songButton = createButton(m_buttonMenu, "Same song", menu_selector(CustomLevelSearchLayer::onSong), 0, -48, (int)(120*0.6), 44*0.6f, 0.6f);
    songButton->setID("song-button"_spr);

    return true;
}

CCMenuItemSpriteExtra* CustomLevelSearchLayer::createButton(CCNode* menu, const char* text, cocos2d::SEL_MenuHandler handler, float x, float y, int width, float height, float scale){
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