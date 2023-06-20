#include <Geode/Bindings.hpp>
#include <Geode/Modify.hpp>

#include "../layers/CustomLevelSearchLayer.h"
#include "../layers/JumpToPageLayer.h"
#include "../layers/UnregisteredProfileLayer.h"

using namespace geode::prelude;

class $modify(BIInfoLayer, InfoLayer) {
    void onJumpToPageLayer(CCObject* sender) {
        JumpToPageLayer::create(this)->show();
    }

    void onCustomSearch(CCObject* sender) {
        CustomLevelSearchLayer::create(m_level)->show();
    }

    void onInfoLayerSchedule(float dt){
        bool prevVisible = this->m_leftArrow->isVisible();
        bool nextVisible = this->m_rightArrow->isVisible();

        auto GLM = GameLevelManager::sharedState();
        if(this->m_commentHistory) GLM->resetCommentTimersForLevelID(this->m_score->m_userID, this->m_commentHistory);
        else GLM->resetCommentTimersForLevelID(this->m_level->m_levelID, this->m_commentHistory);
        this->loadPage(this->m_page, true);

        this->m_loadingCircle->setVisible(false);

        this->m_leftArrow->setVisible(prevVisible);
        this->m_rightArrow->setVisible(nextVisible);
    }

    void onInfoLayerToggleSchedule(CCObject* sender) {
        onInfoLayerSchedule(0);

        auto senderBtn = static_cast<CCMenuItemToggler*>(sender);

        bool newState = !(reinterpret_cast<int>(senderBtn->getUserData()));
        senderBtn->setUserData(reinterpret_cast<void*>(newState));

        const char* text = "Live Comment Refresh: Enabled";
        if(newState) {
            this->getScheduler()->scheduleSelector(schedule_selector(BIInfoLayer::onInfoLayerSchedule), this, 2, false);
        } else {
            text = "Live Comment Refresh: Disabled";
            this->getScheduler()->unscheduleSelector(schedule_selector(BIInfoLayer::onInfoLayerSchedule), this);
        }

        this->addChild(TextAlertPopup::create(text, 0.5f, 0.6f), 100);
    }

    bool init(GJGameLevel* level, GJUserScore* score) {
        if (!InfoLayer::init(level, score)) return false;

        //geode doesn't have ids defined for this so i have to resort to objectAtIndex
        auto layer = static_cast<CCLayer*>(this->getChildren()->objectAtIndex(0));
        CCMenu* menu = static_cast<CCMenu*>(layer->getChildren()->objectAtIndex(1));

        CCMenuItemSpriteExtra* playerName = static_cast<CCMenuItemSpriteExtra*>(menu->getChildren()->objectAtIndex(0));
        playerName->setEnabled(true);

        auto buttonSprite = ButtonSprite::create("1", 12, true, "bigFont.fnt", "GJ_button_02.png", 25, 0.4f);
        auto buttonButton = CCMenuItemSpriteExtra::create(
            buttonSprite,
            this,
            menu_selector(BIInfoLayer::onJumpToPageLayer)
        );
        buttonButton->setSizeMult(1.2f);
        buttonButton->setPosition({195,34});
        buttonButton->setID("bi-comment-page-btn");
        //buttonButton->setTag(commentPageBtnTag);
        menu->addChild(buttonButton);

        if(level == nullptr) return true;

        auto searchSprite = CCSprite::createWithSpriteFrameName("gj_findBtn_001.png");
        searchSprite->setScale(0.8f);
        auto searchButton = CCMenuItemSpriteExtra::create(
            searchSprite,
            this,
            menu_selector(BIInfoLayer::onCustomSearch)
        );
        menu->addChild(searchButton);
        searchButton->setPosition({195, 68});
        searchButton->setSizeMult(1.2f);
        searchButton->setID("bi-search-btn");

        auto scheduleOffSprite = CCSprite::createWithSpriteFrameName("GJ_playEditorBtn_001.png");
        scheduleOffSprite->setScale(.625f);
        auto scheduleOnSprite = CCSprite::createWithSpriteFrameName("GJ_stopEditorBtn_001.png");
        scheduleOnSprite->setScale(.625f);
        auto scheduleBtn = CCMenuItemToggler::create(
            scheduleOffSprite, 
            scheduleOnSprite, 
            this,
            menu_selector(BIInfoLayer::onInfoLayerToggleSchedule)
        );
        scheduleBtn->setUserData(reinterpret_cast<void*>(false));
        scheduleBtn->setPosition({202.5, 100});
        menu->addChild(scheduleBtn);
        scheduleBtn->setID("bi-schedule-btn");

        if(level->m_originalLevel != 0) {
            auto originalBtn = typeinfo_cast<CCNode*>(menu->getChildren()->objectAtIndex(1));
            if(!originalBtn || originalBtn->getPositionY() != 99) return true;

            if(originalBtn->getPositionX() > 155.4f) originalBtn->setPosition({155.4f, originalBtn->getPositionY()});
        }

        return true;
    }

    void onMore(CCObject* sender) {
        if(this->m_level->m_accountID == 0) {
            UnregisteredProfileLayer::displayProfile(this->m_level->m_userID, this->m_level->m_creatorName);
            return;
        }

        InfoLayer::onMore(sender);
    }
};