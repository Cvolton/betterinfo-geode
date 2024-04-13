#include <Geode/Geode.hpp>
#include <Geode/modify/LeaderboardsLayer.hpp>

#include "../utils.hpp"

using namespace geode::prelude;

class BI_DLL $modify(BILeaderboardsLayer, LeaderboardsLayer) {

    /*
     * Callbacks
     */

    void onLeaderboardRefresh(CCObject* sender){
        auto GLM = GameLevelManager::sharedState();
        GLM->updateUserScore();
        GLM->m_storedLevels->removeObjectForKey("leaderboard_creator");
        GLM->m_storedLevels->removeObjectForKey("leaderboard_friends");
        GLM->m_storedLevels->removeObjectForKey("leaderboard_global");
        GLM->m_storedLevels->removeObjectForKey("leaderboard_top");

        auto layer = LeaderboardsLayer::create(GLM->m_leaderboardState);
        auto scene = CCScene::create();
        scene->addChild(layer);
        CCDirector::sharedDirector()->replaceScene(scene);
    }

    /*
     * Hooks
     */

    bool init(LeaderboardState state){
        if(!LeaderboardsLayer::init(state)) return false;

        if(auto menu = getChildByID("bottom-right-menu")) { 
            auto refreshBtn = CCMenuItemSpriteExtra::create(
                CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png"),
                this,
                menu_selector(BILeaderboardsLayer::onLeaderboardRefresh)
            );
            refreshBtn->setID("refresh-button"_spr);
            refreshBtn->setZOrder(-2); //this should ensure we stay at the bottom
            
            menu->addChild(refreshBtn);
            menu->updateLayout();
        }

        return true;
    }

};