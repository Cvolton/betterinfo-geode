#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

#include "../layers/UnregisteredProfileLayer.h"
#include "../managers/BetterInfoCache.h"
#include "../utils.hpp"

#include <thread>
#include <locale>

using namespace geode::prelude;

class $modify(LevelInfoLayer) {
    static void onModify(auto& self) {
        auto res = self.setHookPriority("LevelInfoLayer::onViewProfile", 99999);
        res = self.setHookPriority("LevelInfoLayer::onLevelInfo", 99999);
    }

    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        /**
         * Clickable green username
         */
        auto playerName = static_cast<CCMenuItemSpriteExtra*>(this->getChildByID("creator-info-menu")->getChildByID("creator-name"));
        playerName->setEnabled(true);

        /**
         * Store date for InfoLayer
        */
        auto cache = BetterInfoCache::sharedState();
        cache->storeDatesForLevel(this->m_level);

        /**
         * Add exact time label
        */
        auto label = m_lengthLabel;
        if(label) {
            auto bmFont = CCLabelBMFont::create("Loading", "bigFont.fnt");
            bmFont->setID("exact-time"_spr);
            bmFont->setPosition({label->getPositionX() + 1, label->getPositionY() - 2.f}); //193 - 185
            bmFont->setAnchorPoint({0,1});
            bmFont->setScale(0.325f);
            addChild(bmFont);
            label->setPositionY(label->getPositionY() + 6.f);
        }

        return true;
    }

    void updateLabelValues() {
        LevelInfoLayer::updateLabelValues();

        /**
         * Update exact time label
        */
        retain();

        auto bmFont = typeinfo_cast<CCLabelBMFont*>(getChildByID("exact-time"_spr));
        if(bmFont && m_lengthLabel) bmFont->setPosition({m_lengthLabel->getPositionX() + 1, m_lengthLabel->getPositionY() - 8.f});

        std::thread([this](){
            auto wt = m_level->m_timestamp
                ? TimeUtils::workingTime(m_level->m_timestamp / 240)
                : TimeUtils::workingTime(std::round(BetterInfo::timeForLevelString(m_level->m_levelString)));
            //since whatever is done by queueInMainThread is guaranteed to execute after init is finished, this shouldn't result in a race condition
            Loader::get()->queueInMainThread([this, wt]() {
                auto bmFont = typeinfo_cast<CCLabelBMFont*>(getChildByID("exact-time"_spr));
                if(bmFont) {
                    bmFont->setString(fmt::format("{}", wt).c_str());
                }
                release();
            });
        }).detach();

        /**
         * Update download and like labels
        */
        /*m_downloadsLabel->setString(fmt::format(std::locale("en_US.UTF-8"), "{:L}", m_level->m_downloads).c_str());
        m_likesLabel->setString(fmt::format(std::locale("en_US.UTF-8"), "{:L}", m_level->m_likes).c_str());
        m_downloadsLabel->limitLabelWidth(60.f, .5f, 0.f);
        m_likesLabel->limitLabelWidth(60.f, .5f, 0.f);

        log::info("m_songIDs: {}", m_level->m_songIDs);*/
    }

    void onViewProfile(CCObject* sender) {

        if(this->m_level->m_accountID == 0) {
            UnregisteredProfileLayer::displayProfile(this->m_level->m_userID, this->m_level->m_creatorName);
            return;
        }

        LevelInfoLayer::onViewProfile(sender);
    }

    void onLevelInfo(CCObject* sender) {
        LevelProgressDialog::show(this->m_level);
    }
};