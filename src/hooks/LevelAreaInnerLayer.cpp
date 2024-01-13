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
        //sprite->setScale(0.6f);
        sprite->setScale(0.5f);
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
        m_fields->m_menu->setID("info-menu"_spr);
        static_cast<CCNode*>(getChildren()->objectAtIndex(getChildrenCount() - 1))->addChild(m_fields->m_menu);

        /*addInfoBtn(5001, {-58, -106});
        //addInfoBtn(5002, {139, -50});
        addInfoBtn(5002, {68, -51});
        addInfoBtn(5003, {-58, 5});
        //addInfoBtn(5004, {94, 59});
        addInfoBtn(5004, {22, 58});*/

        /*addInfoBtn(5001, {-95.5f, -124});
        addInfoBtn(5002, {102.5f, -70});
        addInfoBtn(5003, {-95.5f, -13});
        addInfoBtn(5004, {57.5f, 41});*/

        addInfoBtn(5001, {-60, -108.5f});
        addInfoBtn(5002, {68, -52.5f});
        addInfoBtn(5003, {-60, 2});
        addInfoBtn(5004, {57.5f, 41});

        return true;
    }
};