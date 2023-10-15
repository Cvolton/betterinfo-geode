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

void LevelIDLayer::onClose(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

int LevelIDLayer::getNumber(){
    int pageNumber = 1;
    try{
        pageNumber = std::stoi(m_textNode->getString());
    }catch(...){}
    return pageNumber;
}

void LevelIDLayer::onOK(cocos2d::CCObject* sender){
    auto level = GJGameLevel::create();
    level->m_levelID = getNumber();

    InfoLayer* infoLayer = InfoLayer::create(level, nullptr);
    infoLayer->show();

    onClose(sender);
}

bool LevelIDLayer::init(){
    bool init = createBasics({220.0f, 150.0f}, menu_selector(LevelIDLayer::onClose), 0.8f);
    if(!init) return false;

    createTextLabel("View Comments", {0,57}, 0.7f, m_buttonMenu, "goldFont.fnt");

    m_textNode = CCTextInputNode::create(110, 30, "Level ID", "bigFont.fnt");
    m_textNode->setLabelPlaceholderColor({0x75, 0xAA, 0xF0});
    m_textNode->setAllowedChars("0123456789");
    m_textNode->setMaxLabelScale(0.7f);
    m_textNode->setMaxLabelWidth(11);
    m_textNode->setPosition({0,6});
    m_buttonMenu->addChild(m_textNode);

    cocos2d::extension::CCScale9Sprite* infoBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f });
    infoBg->setContentSize({220,60});
    infoBg->setColor({123,60,31});
    m_buttonMenu->addChild(infoBg, -1);
    infoBg->setPosition({0,6});
    infoBg->setScale(0.6f);

    auto buttonSprite = ButtonSprite::create("Go", 40, true, "goldFont.fnt", "GJ_button_01.png", 30, 0.8f);
    auto buttonButton = CCMenuItemSpriteExtra::create(
        buttonSprite,
        this,
        menu_selector(LevelIDLayer::onOK)
    );
    buttonButton->setSizeMult(1.2f);
    buttonButton->setPosition({0,-50});
    m_buttonMenu->addChild(buttonButton);

    return true;
}

CCLabelBMFont* LevelIDLayer::createTextLabel(const std::string text, const CCPoint& position, const float scale, CCNode* menu, const char* font){
    CCLabelBMFont* bmFont = CCLabelBMFont::create(text.c_str(), font);
    bmFont->setPosition(position);
    bmFont->setScale(scale);
    menu->addChild(bmFont);
    return bmFont;
}