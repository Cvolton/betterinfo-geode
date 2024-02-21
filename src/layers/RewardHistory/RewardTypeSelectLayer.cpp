#include "RewardTypeSelectLayer.h"
#include "RewardViewLayer.h"
#include "../../utils.hpp"

RewardTypeSelectLayer* RewardTypeSelectLayer::create(){
    auto ret = new RewardTypeSelectLayer();
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

void RewardTypeSelectLayer::onDaily(cocos2d::CCObject* sender)
{
    auto browserLayer = RewardViewLayer::scene(GameStatsManager::sharedState()->m_dailyChests, "Daily");
    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);
    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void RewardTypeSelectLayer::onTreasure(cocos2d::CCObject* sender)
{
    auto browserLayer = RewardViewLayer::scene(GameStatsManager::sharedState()->m_treasureRoomChests, "Treasure");
    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);
    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void RewardTypeSelectLayer::onWeekly(cocos2d::CCObject* sender)
{
    auto browserLayer = RewardViewLayer::scene(GameStatsManager::sharedState()->m_weeklyChest, "Weekly");
    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);
    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void RewardTypeSelectLayer::onOther(cocos2d::CCObject* sender)
{
    auto browserLayer = RewardViewLayer::scene(GameStatsManager::sharedState()->m_miscChests, "Other");
    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);
    CCDirector::sharedDirector()->pushScene(transitionFade);
}

bool RewardTypeSelectLayer::init(){
    if(!CvoltonAlertLayerStub::init({360,180})) return false;

    createTitle("Chest History");

    //auto dailyBtn = BetterInfo::createTextButton(this, "Timely Chests", menu_selector(RewardTypeSelectLayer::onDaily), (int)(120*0.6), 44*0.6f, 0.6f);
    auto dailyBtn = BetterInfo::createSearchButton(this, "Daily", "GJ_timeIcon_001.png", menu_selector(RewardTypeSelectLayer::onDaily), .5f, .75f);
    dailyBtn->setPosition({-75, 8});
    m_buttonMenu->addChild(dailyBtn);

    /*auto dailyIcon = static_cast<SearchButton*>(dailyBtn->getNormalImage())->m_icon;
    dailyIcon->setPosition({dailyIcon->getPositionX() + 1, dailyIcon->getPositionY() + 1});*/

    auto treasureBtn = BetterInfo::createSearchButton(this, "Treasure", "GJ_diamondsIcon_001.png", menu_selector(RewardTypeSelectLayer::onTreasure), .475f, .8f);
    treasureBtn->setPosition({75, 8});
    m_buttonMenu->addChild(treasureBtn);

    auto weeklyBtn = BetterInfo::createSearchButton(this, "Weekly", "diffIcon_06_btn_001.png", menu_selector(RewardTypeSelectLayer::onWeekly), .5f, .575f);
    weeklyBtn->setPosition({-75, -48});
    m_buttonMenu->addChild(weeklyBtn);

    auto otherBtn = BetterInfo::createSearchButton(this, "Other", "GJ_sMagicIcon_001.png", menu_selector(RewardTypeSelectLayer::onOther), .5f);
    otherBtn->setPosition({75, -48});
    m_buttonMenu->addChild(otherBtn);

    return true;
}