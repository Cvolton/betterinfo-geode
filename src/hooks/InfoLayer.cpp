#include <Geode/Bindings.hpp>
#include <Geode/modify/InfoLayer.hpp>

#include "../layers/CustomLevelSearchLayer.h"
#include "../layers/JumpToPageLayer.h"
#include "../layers/UnregisteredProfileLayer.h"
#include "../layers/ExtendedLevelInfo.h"
#include "../managers/BetterInfoCache.h"

using namespace geode::prelude;

class $modify(BIInfoLayer, InfoLayer) {
    static void onModify(auto& self) {
        auto res = self.setHookPriority("InfoLayer::onMore", 99999);
        res = self.setHookPriority("InfoLayer::onLevelInfo", 99999);
    }

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
        switch(m_mode) {
            case CommentKeyType::Level:
                GLM->resetCommentTimersForLevelID(this->m_level->m_levelID, m_mode);
                break;
            case CommentKeyType::User:
                GLM->resetCommentTimersForLevelID(this->m_score->m_userID, m_mode);
                break;
            case CommentKeyType::LevelList:
                GLM->resetCommentTimersForLevelID(- this->m_levelList->m_listID, m_mode);
                break;                
        }

        this->loadPage(this->m_page, true);

        this->m_loadingCircle->setVisible(false);

        this->m_leftArrow->setVisible(prevVisible);
        this->m_rightArrow->setVisible(nextVisible);
    }

    void onInfoLayerToggleSchedule(CCObject* sender) {
        onInfoLayerSchedule(0);

        auto senderBtn = static_cast<CCMenuItemToggler*>(sender);

        bool newState = !(reinterpret_cast<size_t>(senderBtn->getUserData()));
        senderBtn->setUserData(reinterpret_cast<void*>(newState));

        const char* text = "Live Comment Refresh: Enabled";
        if(newState) {
            this->getScheduler()->scheduleSelector(schedule_selector(BIInfoLayer::onInfoLayerSchedule), this, 2, false);
        } else {
            text = "Live Comment Refresh: Disabled";
            this->getScheduler()->unscheduleSelector(schedule_selector(BIInfoLayer::onInfoLayerSchedule), this);
        }

        //this->addChild(TextAlertPopup::create(text, 0.5f, 0.6f), 100);
        Notification::create(text, NotificationIcon::None, .1f)->show();
    }

    bool init(GJGameLevel* level, GJUserScore* score, GJLevelList* list) {
        if (!InfoLayer::init(level, score, list)) return false;

        if(level) {
            auto cache = BetterInfoCache::sharedState();
            cache->storeDatesForLevel(this->m_level);
        }

        if(auto playerName = static_cast<CCMenuItemSpriteExtra*>(m_buttonMenu->getChildByID("creator-button"))) {
            playerName->setEnabled(true);
        }

        if(auto menu = m_mainLayer->getChildByID("refresh-menu")) {
            auto buttonSprite = ButtonSprite::create("1", 12, true, "bigFont.fnt", "GJ_button_02.png", 25, 0.4f);
            auto buttonButton = CCMenuItemSpriteExtra::create(
                buttonSprite,
                this,
                menu_selector(BIInfoLayer::onJumpToPageLayer)
            );
            buttonButton->setSizeMult(1.2f);
            buttonButton->setPosition({195,34});
            buttonButton->setID("comment-page-btn"_spr);
            //buttonButton->setTag(commentPageBtnTag);
            menu->addChild(buttonButton);
            menu->updateLayout();
        }

        if(level == nullptr && list == nullptr) return true;
        //end of profile stuff

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
        m_buttonMenu->addChild(scheduleBtn);
        scheduleBtn->setID("schedule-btn"_spr);

        if(auto originalBtn = m_buttonMenu->getChildByID("original-level-button")) {
            if(originalBtn->getPositionX() > 155.4f) originalBtn->setPosition({155.4f, originalBtn->getPositionY()});
        }

        if(level == nullptr) return true;
        //end of list stuff

        if(auto menu = m_mainLayer->getChildByID("refresh-menu")) {
            auto searchSprite = CCSprite::createWithSpriteFrameName("gj_findBtn_001.png");
            searchSprite->setScale(0.8f);
            auto searchButton = CCMenuItemSpriteExtra::create(
                searchSprite,
                this,
                menu_selector(BIInfoLayer::onCustomSearch)
            );
            menu->addChild(searchButton);
            searchButton->setSizeMult(1.2f);
            searchButton->setID("search-btn"_spr);

            menu->updateLayout();
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

    void onLevelInfo(CCObject* sender) {
        if(!Mod::get()->getSettingValue<bool>("level-info")){
            InfoLayer::onLevelInfo(sender);
            return;
        }

        ExtendedLevelInfo::create(this->m_level)->show();
    }

    void setupCommentsBrowser(CCArray* a3) {

        InfoLayer::setupCommentsBrowser(a3);

        if(this->m_itemCount >= 999) this->m_rightArrow->setVisible(true);
    }

    void loadPage(int page, bool reload) {
        InfoLayer::loadPage(page, reload);

        CCMenu* menu = static_cast<CCMenu*>(this->m_commentsBtn->getParent());

        auto commentBtn = static_cast<CCMenuItemSpriteExtra*>(menu->getChildByID("comment-page-btn"_spr));
        if(commentBtn == nullptr) return;

        auto commentBtnSprite = static_cast<ButtonSprite*>(commentBtn->getChildren()->objectAtIndex(0));
        if(commentBtnSprite == nullptr) return;

        commentBtnSprite->setString(std::to_string(page+1).c_str());
    }
};