//TODO: reverse MoreSearchLayer

/*#include <Geode/Bindings.hpp>
#include <Geode/modify/MoreSearchLayer.hpp>

#include "../layers/LevelFiltering/CvoltonSearchOptions.h"

using namespace geode::prelude;

class $modify(BIMoreSearchLayer, MoreSearchLayer) {
    CCMenuItemSpriteExtra* m_savedBtn = nullptr;
    LevelSettingsObject* m_settings = nullptr;
    int m_songID = 0;

    bool shouldSavedBtnBeVisible(){
        return GameLevelManager::sharedState()->getBoolForKey("customsong_filter") && GameLevelManager::sharedState()->getBoolForKey("enable_songFilter");
    }*/

    /*
     * Callbacks
     */

    /*void onMoreSearchNext(CCObject* sender){
        auto layer = CvoltonSearchOptions::create();
        CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
        this->onClose(sender);
    }

    void onSaved(CCObject* sender){
        auto browser = GJSongBrowser::create(m_fields->m_settings);
        CCScene::get()->addChild(browser);
        browser->setZOrder(CCScene::get()->getHighestChildZ() + 1);
        browser->showLayer(false);
    }

    void onSongIDCheck(float dt){
        if(m_fields->m_settings->m_level->m_songID != m_fields->m_songID){
            m_fields->m_songID = m_fields->m_settings->m_level->m_songID;
            GameLevelManager::sharedState()->setIntForKey(m_fields->m_songID, "song_filter");
            m_enterSongID->setString(std::to_string(m_fields->m_songID));
        }
    }*/

    /*
     * Hooks
     */

    /*void onClose(CCObject* sender){
        if(m_fields->m_settings) m_fields->m_settings->release();

        MoreSearchLayer::onClose(sender);
    }

    bool init(){
        if(!MoreSearchLayer::init()) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto sprite = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
        sprite->setFlipX(true);

        auto nextBtn = CCMenuItemSpriteExtra::create(
            sprite,
            this,
            menu_selector(BIMoreSearchLayer::onMoreSearchNext)
        );
        nextBtn->setPosition({(winSize.width / 2) + 185, - (winSize.height / 2) + 25});
        nextBtn->setID("next-button"_spr);
        m_buttonMenu->addChild(nextBtn);

        auto savedSprite = CCSprite::createWithSpriteFrameName("GJ_savedSongsBtn_001.png");
        savedSprite->setScale(0.8f);
        m_fields->m_savedBtn = CCMenuItemSpriteExtra::create(
            savedSprite,
            this,
            menu_selector(BIMoreSearchLayer::onSaved)
        );
        m_fields->m_savedBtn->setPosition({333, -247});
        m_fields->m_savedBtn->setID("saved-button"_spr);
        m_fields->m_savedBtn->setVisible(shouldSavedBtnBeVisible());
        m_buttonMenu->addChild(m_fields->m_savedBtn);

        m_fields->m_settings = LevelSettingsObject::create();
        m_fields->m_settings->retain();

        m_fields->m_settings->m_level = GJGameLevel::create();
        m_fields->m_settings->m_level->retain();
        m_fields->m_settings->m_level->m_songID = GameLevelManager::sharedState()->getIntForKey("song_filter");
        m_fields->m_songID = m_fields->m_settings->m_level->m_songID;
        log::info("Song ID: {}", m_fields->m_settings->m_level->m_songID);

        this->getScheduler()->scheduleSelector(schedule_selector(BIMoreSearchLayer::onSongIDCheck), this, 0, false);

        return true;
    }

    void onSongMode(CCObject* sender){
        MoreSearchLayer::onSongMode(sender);

        if(m_fields->m_savedBtn) m_fields->m_savedBtn->setVisible(shouldSavedBtnBeVisible());
    }

    void onSongFilter(CCObject* sender){
        MoreSearchLayer::onSongFilter(sender);

        if(m_fields->m_savedBtn) m_fields->m_savedBtn->setVisible(shouldSavedBtnBeVisible());
    }
};*/