#include "RewardTypeSelectLayer.h"
#include "RewardGroupLayer.h"
#include "RewardViewLayer.h"
#include "../../hooks/GJRewardItem.h"
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
    auto groupLayer = RewardGroupLayer::scene(
        "Treasure",
        GameStatsManager::sharedState()->m_treasureRoomChests,
        {
            {"1 Key", "chest_01_02_001.png", [](BIGJRewardItem* item) -> bool { return item->getKeyInt() < 1000; }},
            {"5 Key", "chest_02_02_001.png", [](BIGJRewardItem* item) -> bool { return item->getKeyInt() >= 1000 && item->getKeyInt() < 2000; }},
            {"10 Key", "chest_03_02_001.png", [](BIGJRewardItem* item) -> bool { return item->getKeyInt() >= 2000 && item->getKeyInt() < 3000; }},
            {"25 Key", "chest_04_02_001.png", [](BIGJRewardItem* item) -> bool { return item->getKeyInt() >= 3000 && item->getKeyInt() < 4000; }},
            {"50 Key", "chest_05_02_001.png", [](BIGJRewardItem* item) -> bool { return item->getKeyInt() >= 4000 && item->getKeyInt() < 5000; }},
            {"100 Key", "chest_06_02_001.png", [](BIGJRewardItem* item) -> bool { return item->getKeyInt() >= 5000 && item->getKeyInt() < 6000; }},
            {"Gold Key", "chest_09_02_001.png", [](BIGJRewardItem* item) -> bool { return item->getKeyInt() >= 6000 && item->getKeyInt() < 7000; }},
        }
    );
    auto transitionFade = CCTransitionFade::create(0.5, groupLayer);
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
    auto groupLayer = RewardGroupLayer::scene(
        "Other",
        GameStatsManager::sharedState()->m_miscChests,
        {
            {"Ad", "chest_02_02_001.png", [](BIGJRewardItem* item) -> bool { 
                return item->getKey().size() == 4 && item->getKey()[0] == '0' && item->getKey()[1] == '0' &&
                    item->getKeyInt() >= 12 && item->getKeyInt() <= 21; 
            }},
            {"Gauntlet", "chest_03_02_001.png", [](BIGJRewardItem* item) -> bool { return item->getKey().size() > 0 && item->getKey()[0] == 'g'; }},
            {"Path", "chest_04_02_001.png", [](BIGJRewardItem* item) -> bool { return item->getKey().size() > 0 && item->getKey()[0] == 'p'; }},
            {"Vault", "chest_05_02_001.png", [](BIGJRewardItem* item) -> bool { return item->getKey().size() > 0 && item->getKey()[0] == 'o' && std::string_view(item->getKey()).starts_with("o_secret"); }},
            {"Event", "chest_06_02_001.png", [](BIGJRewardItem* item) -> bool { return item->getKey().size() > 0 && item->getKey()[0] == 'd'; }},
            {"Other", "chest_07_02_001.png", [](BIGJRewardItem* item) -> bool {
                return item->getKey().size() == 0 || !(
                        item->getKey()[0] == 'g' || item->getKey()[0] == 'p'
                        || (item->getKey().size() == 4 && item->getKey()[0] == '0' && item->getKey()[1] == '0' &&
                            item->getKeyInt() >= 12 && item->getKeyInt() <= 21)
                        || (item->getKey().size() > 0 && item->getKey()[0] == 'o' && std::string_view(item->getKey()).starts_with("o_secret"))
                        || (item->getKey().size() > 0 && item->getKey()[0] == 'd')
                    ); 
            }},
        }
    );
    auto transitionFade = CCTransitionFade::create(0.5, groupLayer);
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