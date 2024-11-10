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
        (void) self.setHookPriority("LevelInfoLayer::onViewProfile", 99999);
        (void) self.setHookPriority("LevelInfoLayer::onLevelInfo", 99999);
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
        m_level->retain();
        std::thread([level = this->m_level](){
            thread::setName("Cache Level");

            auto cache = BetterInfoCache::sharedState();
            cache->cacheLevel(level);
            cache->storeDatesForLevel(level);

            Loader::get()->queueInMainThread([level]() {
                level->release();
            });
        }).detach();

        /**
         * Update exact time label
        */
        this->retain();
        //auto bmFont = typeinfo_cast<CCLabelBMFont*>(getChildByID("exact-time"_spr));
        //if(bmFont && m_lengthLabel) bmFont->setPosition({m_lengthLabel->getPositionX() + 1, m_lengthLabel->getPositionY() - 8.f});

        std::thread([this](){
            thread::setName("Exact Level Length");

            auto wt = m_level->m_timestamp
                ? TimeUtils::workingTime(m_level->m_timestamp / 240)
                : TimeUtils::workingTime(m_level->isPlatformer() ? 0 : std::ceil(BetterInfo::timeForLevelString(m_level->m_levelString)));
            //since whatever is done by queueInMainThread is guaranteed to execute after init is finished, this shouldn't result in a race condition
            Loader::get()->queueInMainThread([this, wt]() {
                m_exactLengthLabel->setString(fmt::format("{}", wt).c_str());
                m_exactLengthLabel->setVisible(true);
                this->release();
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
        static_assert(&LevelInfoLayer::onLevelInfo, "Hook not implemented");

        LevelProgressDialog::show(this->m_level);
    }
};