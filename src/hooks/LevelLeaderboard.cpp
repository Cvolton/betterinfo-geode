//TODO: reverse LevelLeaderboard

#include <Geode/Bindings.hpp>
#include <Geode/modify/LevelLeaderboard.hpp>

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
        level->m_levelLength = this->m_level->m_levelLength;
        GameLevelManager::sharedState()->m_timerDict->removeAllObjects(); //this is equivalent to GLM::resetAllTimers() which I suspect is inlined on windows but i didn't bother checking
        auto newLeaderboard = LevelLeaderboard::create(level, this->m_type, this->m_mode);
        newLeaderboard->onUpdate(sender);
        CCDirector::sharedDirector()->getRunningScene()->addChild(newLeaderboard);
        this->onClose(sender);
    }

    /*
     * Hooks
     */

    bool init(GJGameLevel* level, LevelLeaderboardType type, LevelLeaderboardMode mode) {
        if(!LevelLeaderboard::init(level, type, mode)) return false;

        //daily btn disabled for editor levels cuz it makes no sense there AND for platformer levels cuz it's not implemented server-side
        if(m_level->m_levelType != GJLevelType::Editor && !m_level->isPlatformer()) {

            if(auto secondMenu = m_mainLayer->getChildByID("type-menu")) {
                auto buttonSprite = ButtonSprite::create((level->m_dailyID ? "Daily" : "Normal"), 20, true, "bigFont.fnt", "GJ_button_01.png", 30, 0.5);
                auto buttonButton = CCMenuItemSpriteExtra::create(
                    buttonSprite,
                    this,
                    menu_selector(BILevelLeaderboard::onLeaderboardDaily)
                );
                buttonButton->setID("daily-button"_spr);

                secondMenu->addChild(buttonButton);
                secondMenu->updateLayout();
            }
            
        }

        return true;
    }
};