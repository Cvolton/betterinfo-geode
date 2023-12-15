#include <Geode/Bindings.hpp>
#include <Geode/modify/MoreSearchLayer.hpp>

#include "../layers/LevelFiltering/CvoltonSearchOptions.h"

using namespace geode::prelude;

class $modify(BIMoreSearchLayer, MoreSearchLayer) {

    /*
     * Callbacks
     */

    void onMoreSearchNext(CCObject* sender){
        auto layer = CvoltonSearchOptions::create();
        CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
        this->onClose(sender);
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
        nextBtn->setPosition({(winSize.width / 2) + 185, - (winSize.height / 2) + 25});
        nextBtn->setID("bi-next-button");
        m_buttonMenu->addChild(nextBtn);

        return true;
    }
};