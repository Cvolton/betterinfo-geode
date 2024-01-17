#include "CvoltonOptionsLayer.h"

CvoltonOptionsLayer* CvoltonOptionsLayer::create(){
    auto ret = new CvoltonOptionsLayer();
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

void CvoltonOptionsLayer::onClose(cocos2d::CCObject* sender)
{   
    destroyToggles();
    CvoltonAlertLayerStub::onClose(sender);
}

bool CvoltonOptionsLayer::init(){
    if(!CvoltonAlertLayerStub::init({370.0f, 280.0f}, 0.8f)) return false;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    createTextLabel("Mod Settings", {(winSize.width / 2),(winSize.height / 2) + 78 + 20 + 20}, 0.7f, m_mainLayer, "goldFont.fnt");

    drawToggles();

    return true;
}

CCLabelBMFont* CvoltonOptionsLayer::createTextLabel(const std::string text, const CCPoint& position, const float scale, CCNode* menu, const char* font){
    CCLabelBMFont* bmFont = CCLabelBMFont::create(text.c_str(), font);
    bmFont->setPosition(position);
    bmFont->setScale(scale);
    menu->addChild(bmFont);
    return bmFont;
}

void CvoltonOptionsLayer::onToggle(cocos2d::CCObject* sender)
{
    sender->retain();
    auto button = static_cast<CCMenuItemSpriteExtra*>(sender);
    toggleOption(static_cast<CCString*>(button->getUserObject())->getCString());

    destroyToggles();
    drawToggles();
    sender->release();
}

void CvoltonOptionsLayer::createToggle(const char* option, const char* name, SEL_MenuHandler additional){
    float width = m_alertSize.x - 50;

    float x = -170 + floor(width / m_columns) * floor(m_toggleCount % m_columns);
    float y = m_top - m_gap * floor(m_toggleCount / m_columns);

    createToggle(option, name, x, y, additional);

    m_toggleCount++;
}

void CvoltonOptionsLayer::createToggle(const char* option, const char* name, float x, float y, SEL_MenuHandler additional){
    auto buttonSprite = CCSprite::createWithSpriteFrameName(getOption(option) ? "GJ_checkOn_001.png" : "GJ_checkOff_001.png");
    buttonSprite->setScale(0.8f);
    auto button = CCMenuItemSpriteExtra::create(
        buttonSprite,
        this,
        menu_selector(CvoltonOptionsLayer::onToggle)
    );
    button->setID(Mod::get()->expandSpriteName(fmt::format("{}-toggle", option).c_str()));
    button->setPosition({x, y});
    m_buttonMenu->addChild(button);
    m_toggles.push_back(button);

    auto optionString = CCString::create(option);
    button->setUserObject(optionString);

    auto label = createTextLabel(name, {x + 20, y}, 0.5f, m_buttonMenu);
    label->setAnchorPoint({0,0.5f});
    label->limitLabelWidth(80, 0.35f, 0);
    label->setID(Mod::get()->expandSpriteName(fmt::format("{}-label", option).c_str()));
    m_toggles.push_back(label);

    if(getOption(option) && additional) {
        auto button = createButton("GJ_plusBtn_001.png", {x + 101, y}, additional, .5f);
        m_toggles.push_back(button);
        label->limitLabelWidth(68, 0.35f, 0);
    }
}

void CvoltonOptionsLayer::createButtonToggle(const char* option, CCNode* sprite, float x, float y, float scale){
    
    sprite->setScale(scale);
    auto button = CCMenuItemSpriteExtra::create(
        sprite,
        this,
        menu_selector(CvoltonOptionsLayer::onToggle)
    );
    button->setID(Mod::get()->expandSpriteName(fmt::format("{}-toggle", option).c_str()));
    button->setPosition({x, y});
    if(!getOption(option)) button->setColor({125,125,125});
    m_buttonMenu->addChild(button);
    m_toggles.push_back(button);

    auto optionString = CCString::create(option);
    button->setUserObject(optionString);
}

void CvoltonOptionsLayer::destroyToggles(){
    for(auto toggle : m_toggles) {
        toggle->removeFromParent();
    }
    m_toggles.clear();
    m_toggleCount = 0;
}

void CvoltonOptionsLayer::drawToggles(){
    //createToggle("no_update_check", "Disable Update Check");
    /*createToggle("no_green_user", "Disable Green Username Fix");
    createToggle("no_level_info", "Disable Extended Level Info");
    createToggle("white_id", "White Level ID text");
    createToggle("auto_submit", "Auto submit to level leaderboards");
    createToggle("alt_button", "Alt BI button texture (req. restart)");
    createToggle("show_comment_ids", "Show Comment IDs");*/
}

bool CvoltonOptionsLayer::getOption(const std::string& option) const {
    return Mod::get()->getSavedValue<bool>(option);
}

int CvoltonOptionsLayer::getOptionInt(const std::string& option) const {
    return Mod::get()->getSavedValue<int>(option);
}

bool CvoltonOptionsLayer::toggleOption(const std::string& option) {
    Mod::get()->setSavedValue<bool>(option, !Mod::get()->getSavedValue<bool>(option));
    return Mod::get()->getSavedValue<bool>(option);
}

void CvoltonOptionsLayer::setOption(const std::string& option, bool value) {
    Mod::get()->setSavedValue<bool>(option, value);
}

void CvoltonOptionsLayer::setOptionInt(const std::string& option, int value) {
    Mod::get()->setSavedValue<int>(option, value);
}