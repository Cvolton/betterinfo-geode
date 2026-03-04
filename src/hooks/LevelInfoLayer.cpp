#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

#include "../layers/UnregisteredProfileLayer.h"
#include "../managers/BetterInfoCache.h"
#include "../utils.hpp"

#include <thread>
#include <locale>

using namespace geode::prelude;

class BI_DLL $modify(LevelInfoLayer) {
    static void onModify(auto& self) {
        (void) self.setHookPriority("LevelInfoLayer::onViewProfile", Priority::VeryLatePre);
        (void) self.setHookPriority("LevelInfoLayer::onLevelInfo", Priority::VeryLatePre);
    }

    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        /**
         * Clickable green username
         */
        auto playerName = static_cast<CCMenuItemSpriteExtra*>(this->getChildByID("creator-info-menu")->getChildByID("creator-name"));
        playerName->setEnabled(true);

        /**
         * Add exact time label
        */
        if(!m_exactLengthLabel->isVisible()) {
            m_exactLengthLabel->setVisible(true);
            m_exactLengthLabel->setPositionY(m_exactLengthLabel->getPositionY() + 6.f);
            m_lengthLabel->setPositionY(m_lengthLabel->getPositionY() + 6.f);
        }

        /*auto label = m_lengthLabel;
        if(label) {
            auto bmFont = CCLabelBMFont::create("Loading", "bigFont.fnt");
            bmFont->setID("exact-time"_spr);
            bmFont->setPosition({label->getPositionX() + 1, label->getPositionY() - 2.f}); //193 - 185
            bmFont->setAnchorPoint({0,1});
            bmFont->setScale(0.325f);
            addChild(bmFont);
            label->setPositionY(label->getPositionY() + 6.f);
        }*/

        return true;
    }

    void updateLabelValues() {
        LevelInfoLayer::updateLabelValues();

        /**
         * Cache level
        */
        BetterInfoCache::sharedState()->cacheLevel(m_level);
        if(!m_level->m_uploadDate.empty()) BetterInfoCache::sharedState()->cacheLevelDates(m_level);

        /**
         * Update exact time label
        */
        async::spawn([this]() -> arc::Future<std::string> {
            co_return m_level->m_timestamp
                ? TimeUtils::workingTime(m_level->m_timestamp / 240)
                : TimeUtils::workingTime(m_level->isPlatformer() ? 0 : std::ceil(BetterInfo::timeForLevelString(m_level->m_levelString)));
        }, [self = Ref(this)](std::string wt) {
            self->m_exactLengthLabel->setString(fmt::format("{}", wt).c_str());
            self->m_exactLengthLabel->setVisible(true);
        });
    }

    void onViewProfile(CCObject* sender) {

        if(this->m_level->m_accountID == 0) {
            UnregisteredProfileLayer::displayProfile(this->m_level->m_userID, this->m_level->m_creatorName);
            return;
        }

        LevelInfoLayer::onViewProfile(sender);
    }

    void onLevelInfo(CCObject* sender) {
        static_assert(&LevelInfoLayer::onLevelInfo, "Hook not implemented");

        LevelProgressDialog::show(this->m_level);
    }
};