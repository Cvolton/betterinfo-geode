#include <Geode/Bindings.hpp>
#include <Geode/Modify/LevelLeaderboard.hpp>

using namespace geode::prelude;

class $modify(BILevelLeaderboard, LevelLeaderboard) {

    /*
     * Callbacks
     */

    void onLeaderboardDaily(CCObject* sender){
        auto level = GJGameLevel::create();
        level->m_levelID = this->m_level->m_levelID;
        level->m_levelName = this->m_level->m_levelName;
        level->m_dailyID = !this->m_level->m_dailyID;
        GameLevelManager::sharedState()->m_timerDict->removeAllObjects(); //this is equivalent to GLM::resetAllTimers() which I suspect is inlined on windows but i didn't bother checking
        auto newLeaderboard = LevelLeaderboard::create(level, this->m_type);
        newLeaderboard->onUpdate(sender);
        CCDirector::sharedDirector()->getRunningScene()->addChild(newLeaderboard);
        this->onClose(sender);
    }

    /*
     * Hooks
     */

    bool init(GJGameLevel* level, LevelLeaderboardType type) { //type is usually an enum but i dont have that rn
        if(!LevelLeaderboard::init(level, type)) return false;

        auto buttonSprite = ButtonSprite::create((level->m_dailyID ? "Daily" : "Normal"), 20, true, "bigFont.fnt", "GJ_button_01.png", 30, 0.5);
        auto buttonButton = CCMenuItemSpriteExtra::create(
            buttonSprite,
            this,
            menu_selector(BILevelLeaderboard::onLeaderboardDaily)
        );
        buttonButton->setSizeMult(1.2f);
        buttonButton->setPosition({196,-87});
        m_buttonMenu->addChild(buttonButton);
        buttonButton->setID("bi-daily-button");

        return true;
    }
};