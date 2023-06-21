#include "RewardTypeSelectLayer.h"
#include "RewardViewLayer.h"
#include "../utils.hpp"

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

void RewardTypeSelectLayer::onClose(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
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
    bool init = createBasics({360,180}, menu_selector(RewardTypeSelectLayer::onClose));
    if(!init) return false;

    createTitle("Chest History");

    auto dailyBtn = BetterInfo::createTextButton(this, "Daily", menu_selector(RewardTypeSelectLayer::onDaily), (int)(120*0.6), 44*0.6f, 0.6f);
    dailyBtn->setPosition({-75, 8});
    m_buttonMenu->addChild(dailyBtn);

    auto treasureBtn = BetterInfo::createTextButton(this, "Treasure Room", menu_selector(RewardTypeSelectLayer::onTreasure), (int)(120*0.6), 44*0.6f, 0.6f);
    treasureBtn->setPosition({75, 8});
    m_buttonMenu->addChild(treasureBtn);

    auto weeklyBtn = BetterInfo::createTextButton(this, "Weekly Level", menu_selector(RewardTypeSelectLayer::onWeekly), (int)(120*0.6), 44*0.6f, 0.6f);
    weeklyBtn->setPosition({-75, -48});
    m_buttonMenu->addChild(weeklyBtn);

    auto otherBtn = BetterInfo::createTextButton(this, "Other", menu_selector(RewardTypeSelectLayer::onOther), (int)(120*0.6), 44*0.6f, 0.6f);
    otherBtn->setPosition({75, -48});
    m_buttonMenu->addChild(otherBtn);

    return true;
}