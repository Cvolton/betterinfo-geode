#include <Geode/Bindings.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

#include "../layers/UnregisteredProfileLayer.h"
#include "../layers/ExtendedLevelInfo.h"
#include "../managers/BetterInfoCache.h"
#include "../utils.hpp"

#include <thread>

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

        auto cache = BetterInfoCache::sharedState();
        cache->storeDatesForLevel(this->m_level);

        //auto label = typeinfo_cast<CCLabelBMFont*>(getChildByID("length-label"));
        auto label = m_lengthLabel;
        if(label) {
            auto bmFont = CCLabelBMFont::create("Loading", "bigFont.fnt");
            bmFont->setID("bi-exact-time");
            bmFont->setPosition({label->getPositionX() + 1, label->getPositionY() - 2.f}); //193 - 185
            bmFont->setAnchorPoint({0,1});
            bmFont->setScale(0.325f);
            addChild(bmFont);
            label->setPositionY(label->getPositionY() + 6.f);
        }

        return true;
    }

    void updateLabelValues() {
        retain();

        LevelInfoLayer::updateLabelValues();

        auto bmFont = typeinfo_cast<CCLabelBMFont*>(getChildByID("bi-exact-time"));
        if(bmFont && m_lengthLabel) bmFont->setPosition({m_lengthLabel->getPositionX() + 1, m_lengthLabel->getPositionY() - 8.f});

        std::thread([this](){
            auto wt = ExtendedLevelInfo::workingTime(std::round(BetterInfo::timeForLevelString(m_level->m_levelString)));
            //since whatever is done by queueInMainThread is guaranteed to execute after init is finished, this shouldn't result in a race condition
            Loader::get()->queueInMainThread([this, wt]() {
                //auto label = typeinfo_cast<CCLabelBMFont*>(getChildByID("length-label"));
                auto bmFont = typeinfo_cast<CCLabelBMFont*>(getChildByID("bi-exact-time"));
                if(bmFont) {
                    bmFont->setString(fmt::format("{}", wt).c_str());
                    //label->setString(fmt::format("{} ({})", label->getString(), wt).c_str());
                }
                release();
            });
        }).detach();
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