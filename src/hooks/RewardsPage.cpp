#include <Geode/Geode.hpp>
#include <Geode/modify/RewardsPage.hpp>

#include "../utils.hpp"
#include "../layers/RewardHistory/RewardTypeSelectLayer.h"

using namespace geode::prelude;

class $modify(BIRewardsPage, RewardsPage) {
    CCLabelBMFont* m_timeLabel = nullptr;

    /*
     * Callbacks
     */
    void onRewardsPageHistory(CCObject* sender){
        RewardTypeSelectLayer::create()->show();
    }

    void onTimeRefresh(float dt){
        if(!m_fields->m_timeLabel) return;

        double time = 0xFFFFF - TimeUtils::getRobTopTime();
        m_fields->m_timeLabel->setString(
            fmt::format("Time until timer bug: {}", GameToolbox::getTimeString(time)).c_str()
        );
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
        historyBtn->setPosition({-147,-87});
        historyBtn->setID("history-button"_spr);
        m_buttonMenu->addChild(historyBtn);

        // The bug does not occur on Android,
        // see TimeUtils::getRobTopTime for more information
        #ifndef GEODE_IS_ANDROID
            m_fields->m_timeLabel = CCLabelBMFont::create("Time until timer bug: 3 years", "goldFont.fnt");
            m_fields->m_timeLabel->setScale(.45f);
            m_fields->m_timeLabel->setPosition({0,-95});
            m_fields->m_timeLabel->setVisible(false);
            m_buttonMenu->addChild(m_fields->m_timeLabel);

            this->getScheduler()->scheduleSelector(schedule_selector(BIRewardsPage::onTimeRefresh), this, 1, false);
            onTimeRefresh(0);
        #endif

        return true;
    }
};