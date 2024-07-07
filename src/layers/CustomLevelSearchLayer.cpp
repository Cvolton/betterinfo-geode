#include "CustomLevelSearchLayer.h"
#include "_bases/CvoltonAlertLayerStub.h"

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
    if(!CvoltonAlertLayerStub::init({360.f, 180.f})) return false;

    this->m_level = level;

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