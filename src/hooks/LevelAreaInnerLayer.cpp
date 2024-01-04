#include <Geode/Bindings.hpp>
#include <Geode/modify/LevelAreaInnerLayer.hpp>

#include "../utils.hpp"

using namespace geode::prelude;

class $modify(BILevelAreaInnerLayer, LevelAreaInnerLayer) {
    CCMenu* m_menu = nullptr;

    /*
     * Callbacks
     */
    void onLevelInfo(CCObject* sender){
        auto GLM = GameLevelManager::sharedState();
        auto level = GLM->m_mainLevels->objectForKey(std::to_string(static_cast<CCMenuItemSpriteExtra*>(sender)->getTag()));
        if(level) {
            LevelProgressDialog::show(static_cast<GJGameLevel*>(level));
        }
        /*auto layer = DailyViewLayer::scene(m_type == GJTimedLevelType::Weekly);
        auto transitionFade = CCTransitionFade::create(0.5, layer);
        CCDirector::sharedDirector()->pushScene(transitionFade);*/
    }

    /**
     * Helpers
    */
    void addInfoBtn(int levelID, CCPoint position) {
        auto sprite = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        sprite->setScale(0.6f);
        auto btn = CCMenuItemSpriteExtra::create(
            sprite,
            this,
            menu_selector(BILevelAreaInnerLayer::onLevelInfo)
        );
        btn->setPosition(position);
        btn->setTag(levelID);
        btn->setID("info-button"_spr);
        m_fields->m_menu->addChild(btn);
    }

    /*
     * Hooks
     */

    bool init(bool idk) {
        if(!LevelAreaInnerLayer::init(idk)) return false;

        m_fields->m_menu = CCMenu::create();
        m_fields->m_menu->setPosition({0, 0});
        m_fields->m_menu->setZOrder(10);
        static_cast<CCNode*>(getChildren()->objectAtIndex(getChildrenCount() - 1))->addChild(m_fields->m_menu);

        addInfoBtn(5001, {-58, -79});
        addInfoBtn(5002, {138, -22});
        addInfoBtn(5003, {-61, 35});
        addInfoBtn(5004, {93, 89});
        

        return true;
    }
};