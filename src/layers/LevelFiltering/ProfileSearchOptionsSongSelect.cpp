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
    m_delegate->onSongDialogClosed(getOption("user_search_song_custom"), songID());
    
    CvoltonOptionsLayer::onClose(sender);
}

bool ProfileSearchOptionsSongSelect::init(SongDialogCloseDelegate* delegate){
    if(!CvoltonAlertLayerStub::init({240.0f, 150.0f}, .8f, 0x96)) return false;

    m_delegate = delegate;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    createTitle("Song Filter", .45f, .9f);

    m_textNode = CCTextInputNode::create(100, 30, "Num", "bigFont.fnt");
    m_textNode->setLabelPlaceholderColor({0x75, 0xAA, 0xF0});
    m_textNode->setString(std::to_string(getOptionInt("user_search_song_id")).c_str());
    m_textNode->setAllowedChars("0123456789");
    m_textNode->setMaxLabelScale(0.7f);
    m_textNode->setMaxLabelWidth(90);
    m_textNode->setPosition({0,-37});
    m_textNode->setID("song-id-input"_spr);
    m_buttonMenu->addChild(m_textNode);

    auto infoBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f });
    infoBg->setContentSize({200,60});
    infoBg->setColor({123,60,31});
    infoBg->setPosition({0,-37});
    infoBg->setScale(0.6f);
    infoBg->setID("song-id-bg"_spr);
    m_buttonMenu->addChild(infoBg, -1);

    drawToggles();

    return true;
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
    m_buttonMenu->addChild(m_savedBtn);
    m_toggles.push_back(m_savedBtn);
}

int ProfileSearchOptionsSongSelect::songID(){
    return BetterInfo::stoi(m_textNode->getString());
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