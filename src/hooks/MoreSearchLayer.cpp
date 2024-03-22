#include <Geode/Geode.hpp>
#include <Geode/modify/MoreSearchLayer.hpp>

#include "../layers/LevelFiltering/CvoltonSearchOptions.h"

using namespace geode::prelude;

class DropDownDelegateHelper : public GJDropDownLayerDelegate {
public:
    Ref<CCTextInputNode> m_input = nullptr;

    virtual void dropDownLayerWillClose(GJDropDownLayer* layer) {
        auto browser = static_cast<GJSongBrowser*>(layer);
        browser->m_delegate = nullptr;

        if(m_input) m_input->setString(std::to_string(browser->m_songID).c_str());

        delete this;
    }
};

class $modify(BIMoreSearchLayer, MoreSearchLayer) {
    CCMenuItemSpriteExtra* m_savedBtn = nullptr;
    int m_songID = 0;

    bool shouldSavedBtnBeVisible(){
        return GameLevelManager::sharedState()->getBoolForKey("customsong_filter") && GameLevelManager::sharedState()->getBoolForKey("enable_songFilter");
    }

    /*
     * Callbacks
     */

    void onMoreSearchNext(CCObject* sender){
        auto layer = CvoltonSearchOptions::create();
        CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
        this->onClose(sender);
    }

    void onSaved(CCObject* sender){
        auto delegate = new DropDownDelegateHelper();
        delegate->m_input = m_enterSongID;

        auto browser = GJSongBrowser::create();
        CCScene::get()->addChild(browser);
        browser->setZOrder(CCScene::get()->getHighestChildZ() + 1);
        browser->showLayer(false);
        browser->m_delegate = delegate;
    }

    /*
     * Hooks
     */

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
        nextBtn->setPosition({(winSize.width / 2) + 185, - 135});
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
};