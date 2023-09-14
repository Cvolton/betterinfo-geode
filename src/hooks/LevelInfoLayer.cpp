#include <Geode/Bindings.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

#include "../layers/UnregisteredProfileLayer.h"
#include "../layers/ExtendedLevelInfo.h"
#include "../managers/BetterInfoCache.h"
#include "../utils.hpp"

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

        std::thread([this](){
            auto wt = ExtendedLevelInfo::workingTime(std::round(BetterInfo::timeForLevelString(m_level->m_levelString)));
            Loader::get()->queueInMainThread([this, wt]() {
                auto label = typeinfo_cast<CCLabelBMFont*>(getChildByID("length-label"));
                if(label) {
                    //label->setString(fmt::format("{} ({})", label->getString(), wt).c_str());
                    auto bmFont = CCLabelBMFont::create(fmt::format("{}", wt).c_str(), "bigFont.fnt");
                    bmFont->setID("bi-exact-time");
                    bmFont->setPosition({label->getPositionX() + 1, label->getPositionY() - 2.f}); //todo: position properly
                    label->setPositionY(label->getPositionY() + 6.f);

                    //193 - 185
                    bmFont->setAnchorPoint({0,1});
                    bmFont->setScale(0.325f);
                    addChild(bmFont);
                }
            });
        }).detach();
        

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