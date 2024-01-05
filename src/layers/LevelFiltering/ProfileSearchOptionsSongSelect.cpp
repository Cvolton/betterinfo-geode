#include "ProfileSearchOptionsSongSelect.h"

const int completedMax = 6;

ProfileSearchOptionsSongSelect* ProfileSearchOptionsSongSelect::create(SongDialogCloseDelegate* delegate){
    auto ret = new ProfileSearchOptionsSongSelect();
    if (ret && ret->init(delegate)) {
        //robert 1 :D
        ret->autorelease();
    } else {
        //robert -1
        delete ret;
        ret = nullptr;
    }
    return ret;
}

void ProfileSearchOptionsSongSelect::onClose(cocos2d::CCObject* sender)
{
    destroyToggles();
    m_delegate->onSongDialogClosed(getOption("user_search_song_custom"), songID());
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

bool ProfileSearchOptionsSongSelect::init(SongDialogCloseDelegate* delegate){
    bool init = createBasics({240.0f, 150.0f}, menu_selector(ProfileSearchOptionsSongSelect::onClose), .8f, {0x00, 0x00, 0x00, 0x96});
    if(!init) return false;

    m_delegate = delegate;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    createTitle("Song Filter", .45f, .9f);

    m_textNode = CCTextInputNode::create(100, 30, "Num", "bigFont.fnt");
    m_textNode->setLabelPlaceholderColor({0x75, 0xAA, 0xF0});
    m_textNode->setString(std::to_string(getOptionInt("user_search_song_id")).c_str());
    m_textNode->setAllowedChars("0123456789");
    m_textNode->setMaxLabelScale(0.7f);
    m_textNode->setMaxLabelWidth(11);
    m_textNode->setPosition({0,-37});
    m_textNode->setID("song-id-input"_spr);
    m_buttonMenu->addChild(m_textNode);

    auto infoBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f });
    infoBg->setContentSize({200,60});
    infoBg->setColor({123,60,31});
    m_buttonMenu->addChild(infoBg, -1);
    infoBg->setPosition({0,-37});
    infoBg->setScale(0.6f);
    infoBg->setID("song-id-bg"_spr);
    //createTextLabel("Advanced Options", {(winSize.width / 2), (winSize.height / 2) + 125}, 1.f, m_pLayer, "bigFont.fnt");

    drawToggles();

    return true;
}

void ProfileSearchOptionsSongSelect::createToggle(const char* option, const char* name, float x, float y){
    auto buttonSprite = CCSprite::createWithSpriteFrameName(getOption(option) ? "GJ_checkOn_001.png" : "GJ_checkOff_001.png");
    buttonSprite->setScale(0.8f);
    auto button = CCMenuItemSpriteExtra::create(
        buttonSprite,
        this,
        menu_selector(ProfileSearchOptionsSongSelect::onToggle)
    );
    button->setID(Mod::get()->expandSpriteName(fmt::format("{}-toggle", option).c_str()));
    m_buttonMenu->addChild(button);
    button->setPosition({x, y});
    auto optionString = CCString::create(option);
    optionString->retain();
    button->setUserData(optionString);
    button->setSizeMult(1.2f);

    auto label = createTextLabel(name, {x + 20, y}, 0.5f, m_buttonMenu);
    label->setAnchorPoint({0,0.5f});
    label->limitLabelWidth(80, 0.5f, 0);
    label->setID(Mod::get()->expandSpriteName(fmt::format("{}-label", option).c_str()));
}

void ProfileSearchOptionsSongSelect::destroyToggles(){
    //starting at 1 because 0 is the close button and 1 is the prev button
    unsigned int totalMembers = m_buttonMenu->getChildrenCount();
    for(unsigned int i = 5; i < totalMembers; i++){
        //static index 1 because we're actively moving the elements
        auto object = static_cast<CCNode*>(m_buttonMenu->getChildren()->objectAtIndex(5));
        auto userData = object->getUserData();
        if(userData != nullptr) static_cast<CCString*>(userData)->release();
        //m_buttonMenu->removeChild(object, false);
        object->removeFromParent();
    }
    m_toggleCount = 0;
}

void ProfileSearchOptionsSongSelect::drawToggles(){
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    createToggle("user_search_song_custom", "Custom", -40, 6);
    
    auto savedSprite = CCSprite::createWithSpriteFrameName("GJ_savedSongsBtn_001.png");
    savedSprite->setScale(0.8f);
    m_savedBtn = CCMenuItemSpriteExtra::create(
        savedSprite,
        this,
        menu_selector(ProfileSearchOptionsSongSelect::onSaved)
    );
    m_savedBtn->setPosition({88, -39});
    m_savedBtn->setID("saved-button"_spr);
    m_savedBtn->setVisible(getOption("user_search_song_custom"));
    m_savedBtn->setVisible(getOption("user_search_song_custom"));
    m_buttonMenu->addChild(m_savedBtn);
}

int ProfileSearchOptionsSongSelect::songID(){
    int songID = 0;
    try{
        songID = std::stoi(m_textNode->getString());
    }catch(...){}
    return songID;
}

void ProfileSearchOptionsSongSelect::onSaved(CCObject* sender){
    auto browser = GJSongBrowser::create();
    CCScene::get()->addChild(browser);
    browser->m_delegate = this;
    browser->setZOrder(CCScene::get()->getHighestChildZ() + 1);
    browser->showLayer(false);
}

void ProfileSearchOptionsSongSelect::dropDownLayerWillClose(GJDropDownLayer* layer) {
    auto browser = static_cast<GJSongBrowser*>(layer);
    if(browser->m_selected) {
        m_textNode->setString(std::to_string(browser->m_songID).c_str());
    }
}