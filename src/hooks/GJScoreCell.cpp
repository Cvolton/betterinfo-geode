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

            player->setPosition(container->getContentSize() / 2);

            auto playerBtn = CCMenuItemSpriteExtra::create(
                container,
                this,
                menu_selector(BIGJScoreCell::onSimplePlayer)
            );

            playerMenu->addChild(playerBtn);
        }

    }
};