#include <Geode/Bindings.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

#include "../layers/UnregisteredProfileLayer.h"
#include "../layers/ExtendedLevelInfo.h"

using namespace geode::prelude;

class $modify(LevelInfoLayer) {
    static void onModify(auto& self) {
        auto res = self.setHookPriority("LevelInfoLayer::onViewProfile", 99999);
        res = self.setHookPriority("LevelInfoLayer::onLevelInfo", 99999);
    }

    bool init(GJGameLevel* level) {
        if (!LevelInfoLayer::init(level)) return false;

        auto playerName = static_cast<CCMenuItemSpriteExtra*>(this->getChildByID("creator-info-menu")->getChildByID("creator-name"));
        playerName->setEnabled(true);

        return true;
    }

    void onViewProfile(CCObject* sender) {

        if(this->m_level->m_accountID == 0) {
            UnregisteredProfileLayer::displayProfile(this->m_level->m_userID, this->m_level->m_creatorName);
            return;
        }

        LevelInfoLayer::onViewProfile(sender);
    }

    void onLevelInfo(CCObject* sender) {
        ExtendedLevelInfo::showProgressDialog(this->m_level);
    }
};