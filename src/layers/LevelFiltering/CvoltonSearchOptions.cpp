#include "CvoltonSearchOptions.h"
#include "IDRangePopup.h"
#include "../../enums.hpp"

const int completedMax = 7;

CvoltonSearchOptions* CvoltonSearchOptions::create(){
    auto ret = new CvoltonSearchOptions();
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

void CvoltonSearchOptions::onClose(cocos2d::CCObject* sender)
{
    destroyToggles();
    CvoltonAlertLayerStub::onClose(sender);
}

void CvoltonSearchOptions::onInfo(cocos2d::CCObject* sender)
{
    FLAlertLayer::create(nullptr, "Advanced options", "<cl>Trim Spaces:</c> Removes spaces from the beginning and end of your search query\n\n<co>Completed Mode:</c> Changes which levels are sent to the completed/uncompleted filters", "OK", nullptr, 480)->show();
}

void CvoltonSearchOptions::onPrev(cocos2d::CCObject* sender)
{
    auto layer = MoreSearchLayer::create();
    CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
    onClose(sender);
}

bool CvoltonSearchOptions::init(){
    if(!CvoltonAlertLayerStub::init({440.0f, 290.0f}, 1.f, {0x00, 0x00, 0x00, 0x96})) return false;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto label = createTextLabel("Advanced Options", {(winSize.width / 2), (winSize.height / 2) + 125}, 1.f, m_mainLayer, "bigFont.fnt");
    label->setID("title"_spr);
    auto prevBtn = createButton("GJ_arrow_03_001.png", {- (winSize.width / 2) + 30, 0}, menu_selector(CvoltonSearchOptions::onPrev));
    prevBtn->setID("prev-button"_spr);
    auto infoBtn = createButton("GJ_infoIcon_001.png", {203, 128}, menu_selector(CvoltonSearchOptions::onInfo));
    infoBtn->setID("info-button"_spr);

    drawToggles();

    return true;
}

void CvoltonSearchOptions::createToggle(const char* option, const char* name, float x, float y){

    auto buttonSprite = CCSprite::createWithSpriteFrameName(getOption(option) ? "GJ_checkOn_001.png" : "GJ_checkOff_001.png");
    buttonSprite->setScale(0.8f);
    auto button = CCMenuItemSpriteExtra::create(
        buttonSprite,
        this,
        menu_selector(CvoltonSearchOptions::onToggle)
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

void CvoltonSearchOptions::destroyToggles(){
    //starting at 1 because 0 is the close button and 1 is the prev button
    unsigned int totalMembers = m_buttonMenu->getChildrenCount();
    for(unsigned int i = 3; i < totalMembers; i++){
        //static index 1 because we're actively moving the elements
        auto object = static_cast<CCNode*>(m_buttonMenu->getChildren()->objectAtIndex(3));
        auto userData = object->getUserData();
        if(userData != nullptr) static_cast<CCString*>(userData)->release();
        //m_buttonMenu->removeChild(object, false);
        object->removeFromParent();
    }
    m_toggleCount = 0;
}

void CvoltonSearchOptions::drawToggles(){
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    //createToggle("search_contains", "Name Contains", -170, 75); //40 -60, 170 -60, 300 -60, 40 -110
    //createToggle("search_no_id", "No ID Search", -40, 75);
    //createToggle("search_surround_percent", "No Forced Star", 90, 75);
    createToggle("search_trim", "Trim Spaces", -170, 75);

    auto completedMode = createTextLabel("Completed Mode:", {0, -95}, 0.5f, m_buttonMenu, "goldFont.fnt");
    completedMode->setID("completed-mode"_spr);
    auto completedLeft = createButton("edit_leftBtn_001.png", {-120, -120}, menu_selector(CvoltonSearchOptions::onCompletedPrev), 1.2f);
    completedLeft->setID("completed-left-button"_spr);
    auto label = createTextLabel(getCompletedString(), {0, -120}, 1, m_buttonMenu, "bigFont.fnt");
    label->limitLabelWidth(200, 0.8f, 0);
    label->setID("completed-mode-label"_spr);
    auto completedRight = createButton("edit_rightBtn_001.png", {120, -120}, menu_selector(CvoltonSearchOptions::onCompletedNext), 1.2f);
    completedRight->setID("completed-right-button"_spr);
    if(static_cast<CompleteMode>(getOptionInt("search_completed")) == CompleteMode::percentage) {
        auto plus = createButton("GJ_plusBtn_001.png", {196, -120}, menu_selector(CvoltonSearchOptions::onPercentageRange), .75f);
        plus->setID("percentage-plus-button"_spr);
    }
}

void CvoltonSearchOptions::onCompletedPrev(cocos2d::CCObject* sender)
{
    int value = getOptionInt("search_completed") - 1;
    if(value < 0) value = completedMax - 1;
    setOptionInt("search_completed", value);
    destroyToggles();
    drawToggles();
    GameLevelManager::sharedState()->m_timerDict->removeAllObjects();
}

void CvoltonSearchOptions::onCompletedNext(cocos2d::CCObject* sender)
{
    setOptionInt("search_completed", (getOptionInt("search_completed") + 1) % completedMax);
    destroyToggles();
    drawToggles();
    GameLevelManager::sharedState()->m_timerDict->removeAllObjects();
}

void CvoltonSearchOptions::onPercentageRange(CCObject* sender) {
    IDRangePopup::create(this, getOptionInt("search_completed_percentage_min"), getOptionInt("search_completed_percentage_max"), "Percentage")->show();
    GameLevelManager::sharedState()->m_timerDict->removeAllObjects();
}

std::string CvoltonSearchOptions::getCompletedString(){
    int type = getOptionInt("search_completed");
    const char* types[] = {"Default", "Completed Levels", "Completed Orbs", "Completed Leaderboard", "Completed With Coins", "Completed Without Coins", "Percentage"};
    return std::to_string(type) + ": " + types[type % completedMax];
}

void CvoltonSearchOptions::onIDRangeFinished(int min, int max, int additional) {
    setOptionInt("search_completed_percentage_min", min);
    setOptionInt("search_completed_percentage_max", max);
}
