#include <Geode/Bindings.hpp>
#include <Geode/modify/DailyLevelPage.hpp>

#include "../utils.hpp"
#include "../layers/DailyHistory/DailyViewLayer.h"

using namespace geode::prelude;

class $modify(BIDailyLevelPage, DailyLevelPage) {
    /*
     * Callbacks
     */
    void onDailyHistory(CCObject* sender){
        auto layer = DailyViewLayer::scene(this->m_weekly);
        auto transitionFade = CCTransitionFade::create(0.5, layer);
        CCDirector::sharedDirector()->pushScene(transitionFade);
    }

    /*
     * Hooks
     */
    void updateTimers(float something) {
        DailyLevelPage::updateTimers(something);

        CCLayer* layer = this->m_mainLayer;

        if(layer->getChildByID("bi-current-daily-text")) return;

        auto GM = GameLevelManager::sharedState();
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        std::ostringstream currentDaily;
        currentDaily << "Current: #" << ((this->m_weekly) ? GM->m_weeklyID % 100000 : GM->m_dailyID);
        auto currentDailyNode = CCLabelBMFont::create(currentDaily.str().c_str(), "chatFont.fnt");
        currentDailyNode->setPosition({(winSize.width / 2) + 183, (winSize.height / 2) + 51});
        currentDailyNode->setAnchorPoint({1,0});
        currentDailyNode->setScale(0.6f);
        currentDailyNode->setColor({255,255,255});
        currentDailyNode->setOpacity(200);
        layer->addChild(currentDailyNode);
        currentDailyNode->setID("bi-current-daily-text");
    }

    bool init(bool isWeekly) {
        if(!DailyLevelPage::init(isWeekly)) return false;

        CCMenu* menu = this->m_buttonMenu;

        auto historySprite = BetterInfo::createWithBISpriteFrameName("BI_historyBtn_001.png");
        historySprite->setScale(0.8f);
        auto historyBtn = CCMenuItemSpriteExtra::create(
            historySprite,
            this,
            menu_selector(BIDailyLevelPage::onDailyHistory)
        );
        historyBtn->setPosition({13, -247});
        menu->addChild(historyBtn);
        historyBtn->setID("bi-history-button");

        return true;
    }
};