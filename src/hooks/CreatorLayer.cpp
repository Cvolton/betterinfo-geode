#include <Geode/Geode.hpp>
#include <Geode/modify/CreatorLayer.hpp>

#include "../utils.hpp"
#include "../layers/CustomCreatorLayer.h"

using namespace geode::prelude;

class BI_DLL $modify(BICreatorLayer, CreatorLayer) {
    /*
     * Callbacks
     */
    void onCustomCreatorLayer(CCObject* sender){
        auto browserLayer = CustomCreatorLayer::scene();
        auto transitionFade = CCTransitionFade::create(0.5, browserLayer);
        CCDirector::sharedDirector()->pushScene(transitionFade);
    }

    /*
     * Hooks
     */

    bool init() {
        if(!CreatorLayer::init()) return false;

        //betterinfo btn
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto menu = CCMenu::create();
        menu->setPositionX(winSize.width - 22);
        menu->setID("center-right-menu"_spr);

        auto buttonSprite = BetterInfo::createBISprite(Mod::get()->getSettingValue<bool>("alt-button") ? "BI_altBtn_001.png" : "BI_mainButton_001.png");
        buttonSprite->setScale(.9f);
        auto buttonButton = CCMenuItemSpriteExtra::create(
            buttonSprite,
            this,
            menu_selector(BICreatorLayer::onCustomCreatorLayer)
        );
        buttonButton->setSizeMult(1.2f);
        //buttonButton->setPosition({door->getPositionX() - 1,0});
        menu->addChild(buttonButton);
        buttonButton->setID("main-button"_spr);

        this->addChild(menu);

        if(auto updatePopup = BetterInfo::createUpdateDialog()) {
            updatePopup->m_scene = this;
            updatePopup->show();
        }

        Mod::get()->setSavedValue<std::string>("last_launch_version", Mod::get()->getVersion().toVString());

        //showQuestExclamationMark();

        return true;
    }
};