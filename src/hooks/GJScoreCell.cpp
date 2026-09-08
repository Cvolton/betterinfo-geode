#include <Geode/Geode.hpp>
#include <Geode/modify/GJScoreCell.hpp>

#include "../utils.hpp"

using namespace geode::prelude;

class BI_DLL $modify(BIGJScoreCell, GJScoreCell) {

    /*
     * Callbacks
     */
    void onSimplePlayer(CCObject* sender){
        ItemInfoPopup::create(m_score->m_iconID, BetterInfo::iconTypeToUnlockType(m_score->m_iconType))->show();
    }

    /*
     * Hooks
     */

    void loadFromScore(GJUserScore* b) {
        GJScoreCell::loadFromScore(b);

        if(auto player = m_mainLayer->getChildByID("player-icon")) {
            player->removeFromParent();

            auto playerMenu = CCMenu::create();
            playerMenu->setPosition(player->getPosition());
            m_mainLayer->addChild(playerMenu);

            auto container = CCNode::create();
            container->setContentSize({50,50});
            container->addChild(player);
            container->setID("player-icon-container"_spr);

            player->setPosition(container->getContentSize() / 2);

            auto playerBtn = CCMenuItemSpriteExtra::create(
                container,
                this,
                menu_selector(BIGJScoreCell::onSimplePlayer)
            );
            playerBtn->setID("player-icon-button"_spr);

            playerMenu->addChild(playerBtn);
            playerMenu->setID("player-icon-menu"_spr);
        }

        auto timestamp = typeinfo_cast<CCInteger*>(b->getUserObject("key_64"_spr));
        if(timestamp && timestamp->getValue() > 0) {
            auto label = Label::create(
                fmt::format("Friends since: {}", TimeUtils::timeToString(timestamp->getValue(), true)),
                "chatFont.fnt"
            );
            label->setScale(.4f);
            label->setAnchorPoint({1, 0});
            label->setPosition({345, 2});
            label->setColor({51,51,51});
            label->setOpacity(200);
            label->setID("friends-since-label"_spr);
            m_mainLayer->addChild(label);
        }

    }
};