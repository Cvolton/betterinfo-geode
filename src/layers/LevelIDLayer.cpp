#include "LevelIDLayer.h"

LevelIDLayer* LevelIDLayer::create(){
    auto ret = new LevelIDLayer();
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

int LevelIDLayer::getNumber(){
    int pageNumber = 1;
    try{
        pageNumber = std::stoi(m_textNode->getString());
    }catch(...){}
    return pageNumber;
}

void LevelIDLayer::onLevelComments(cocos2d::CCObject* sender){
    auto level = GJGameLevel::create();
    level->m_levelID = getNumber();

    InfoLayer* infoLayer = InfoLayer::create(level, nullptr, nullptr);
    infoLayer->show();

    onClose(sender);
}

void LevelIDLayer::onAccountProfile(cocos2d::CCObject* sender){
    ProfilePage::create(getNumber(), false)->show();
}

bool LevelIDLayer::init(){
    if(!CvoltonAlertLayerStub::init({220.0f, 150.0f}, 0.8f)) return false;

    auto title = createTextLabel("ID Search", {0,57}, 0.7f, m_buttonMenu, "goldFont.fnt");
    title->setID("title-label"_spr);

    m_textNode = CCTextInputNode::create(150, 30, "ID", "bigFont.fnt");
    m_textNode->setLabelPlaceholderColor({0x75, 0xAA, 0xF0});
    m_textNode->setAllowedChars("0123456789");
    m_textNode->setMaxLabelScale(0.7f);
    m_textNode->setMaxLabelWidth(140);
    m_textNode->setPosition({0,6});
    m_textNode->setID("id-input"_spr);
    m_buttonMenu->addChild(m_textNode);

    cocos2d::extension::CCScale9Sprite* infoBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f });
    infoBg->setContentSize({300,60});
    infoBg->setColor({123,60,31});
    m_buttonMenu->addChild(infoBg, -1);
    infoBg->setPosition({0,6});
    infoBg->setScale(0.6f);
    infoBg->setID("id-background"_spr);

    auto buttonSprite = ButtonSprite::create("Lv. Comments", 80, true, "goldFont.fnt", "GJ_button_01.png", 30, 0.8f);
    auto buttonButton = CCMenuItemSpriteExtra::create(
        buttonSprite,
        this,
        menu_selector(LevelIDLayer::onLevelComments)
    );
    buttonButton->setPosition({-52,-45});
    buttonButton->setID("comments-button"_spr);
    m_buttonMenu->addChild(buttonButton);

    auto profileSprite = ButtonSprite::create("Acc. Profile", 80, true, "goldFont.fnt", "GJ_button_01.png", 30, 0.8f);
    auto profileButton = CCMenuItemSpriteExtra::create(
        profileSprite,
        this,
        menu_selector(LevelIDLayer::onAccountProfile)
    );
    profileButton->setPosition({52,-45});
    profileButton->setID("profile-button"_spr);
    m_buttonMenu->addChild(profileButton);

    return true;
}

CCLabelBMFont* LevelIDLayer::createTextLabel(const std::string text, const CCPoint& position, const float scale, CCNode* menu, const char* font){
    CCLabelBMFont* bmFont = CCLabelBMFont::create(text.c_str(), font);
    bmFont->setPosition(position);
    bmFont->setScale(scale);
    menu->addChild(bmFont);
    return bmFont;
}