#include <Geode/Geode.hpp>
#include <Geode/modify/ItemInfoPopup.hpp>

#include "../utils.hpp"

using namespace geode::prelude;

class BI_DLL $modify(BIItemInfoPopup, ItemInfoPopup) {

    /*
     * Callbacks
     */
    const char* chestIDToKeys(int chestID) {
        if(chestID > 6000) return "golden";
        if(chestID > 5000) return "100";
        if(chestID > 4000) return "50";
        if(chestID > 3000) return "25";
        if(chestID > 2000) return "10";
        if(chestID > 1000) return "5";
        return "1";
    }

    void updateLabelString(CCMenuItemSpriteExtra* button, const char* string) {
        auto label = typeinfo_cast<CCLabelBMFont*>(button->getNormalImage());
        if(!label) return;

        auto size = label->getScaledContentSize();

        label->setString(string);
        auto newSize = label->getScaledContentSize();
        auto offset = (size - newSize) / 2;
        button->setPosition(button->getPosition() - offset);
        button->setEnabled(false);
    }

    void onChestReveal(CCObject* sender){
        auto button = typeinfo_cast<CCMenuItemSpriteExtra*>(sender);
        if(!button) return;

        for(auto [key, chest] : CCDictionaryExt<int, GJRewardItem*>(GameStatsManager::sharedState()->m_allTreasureRoomChests)) {
            for(auto reward : CCArrayExt<GJRewardObject*>(chest->m_rewardObjects)) {
                if(reward->m_specialRewardItem == SpecialRewardItem::CustomItem) {
                    if(reward->m_itemID == m_itemID && reward->m_unlockType == m_unlockType) {
                        updateLabelString(button, fmt::format("Found in:\na {} key chest", chestIDToKeys(key)).c_str());
                        return;
                    }
                }
            }
        }

        #ifndef GEODE_IS_ANDROID
        if(GameStatsManager::sharedState()->m_wraithIcons.contains(std::pair(m_unlockType, m_itemID))) {
            updateLabelString(button, "Found in:\na wraith chest");
            return;
        }
        #else
        // use wraith chest as fallback on android because the set is not implemented in gd stl
        updateLabelString(button, "Found in:\na wraith chest");
        #endif
    }

    /*
     * Hooks
     */

    bool init(int iconID, UnlockType unlockType) {
        if(!ItemInfoPopup::init(iconID, unlockType)) return false;

        auto state = GameStatsManager::sharedState()->getItemUnlockState(iconID, unlockType);
        if(state == 2 || state == 3) {
            auto clickToReveal = CCLabelBMFont::create("Found in:\n(Click to Reveal)", "goldFont.fnt");
            clickToReveal->setScale(.4f);
            auto revealButton = CCMenuItemSpriteExtra::create(
                clickToReveal,
                nullptr,
                this,
                menu_selector(BIItemInfoPopup::onChestReveal)
            );
            revealButton->setPosition({-90, 0});
            revealButton->setID("chest-reveal-button"_spr);
            m_buttonMenu->addChild(revealButton);
        }

        return true;
    }
};