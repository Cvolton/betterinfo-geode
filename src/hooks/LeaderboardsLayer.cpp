#include <Geode/Bindings.hpp>
#include <Geode/Modify/LeaderboardsLayer.hpp>

using namespace geode::prelude;

class $modify(BILeaderboardsLayer, LeaderboardsLayer) {

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

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto layer = static_cast<CCLayer*>(this->getChildren()->objectAtIndex(0));

        auto refreshBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png"),
            this,
            menu_selector(BILeaderboardsLayer::onLeaderboardRefresh)
        );

        auto menuRefresh = CCMenu::create();
        menuRefresh->addChild(refreshBtn);
        menuRefresh->setPosition({winSize.width - 26.75f, 26.75f});
        menuRefresh->setZOrder(2);
        menuRefresh->setID("bi-refresh-menu");

        this->addChild(menuRefresh);

        return true;
    }

};