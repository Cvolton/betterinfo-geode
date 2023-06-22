#include <Geode/Bindings.hpp>
#include <Geode/modify/RewardsPage.hpp>

#include "../utils.hpp"
#include "../layers/RewardTypeSelectLayer.h"

using namespace geode::prelude;

class $modify(BIRewardsPage, RewardsPage) {

    /*
     * Callbacks
     */
    void onRewardsPageHistory(CCObject* sender){
        RewardTypeSelectLayer::create()->show();
    }

    /*
     * Hooks
     */

    bool init(){
        if(!RewardsPage::init()) return false;

        auto historySprite = BetterInfo::createWithBISpriteFrameName("BI_historyBtn_001.png");
        historySprite->setScale(0.8f);
        auto historyBtn = CCMenuItemSpriteExtra::create(
            historySprite,
            this,
            menu_selector(BIRewardsPage::onRewardsPageHistory)
        );
        historyBtn->setPosition({-147,-88});
        this->m_buttonMenu->addChild(historyBtn);
        historyBtn->setID("bi-history-button");

        return true;
    }
};