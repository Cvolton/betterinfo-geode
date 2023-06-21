#include <Geode/Bindings.hpp>
#include <Geode/Modify/CreatorLayer.hpp>

#include "../utils.hpp"
#include "../layers/LevelBrowserEndLayer.h"
#include "../layers/LevelSearchViewLayer.h"
#include "../layers/ProfileSearchOptions.h"

using namespace geode::prelude;

class $modify(BICreatorLayer, CreatorLayer) {
    /*
     * Callbacks
     */
    void onCustomCreatorLayer(CCObject* sender){
        /*auto browserLayer = CustomCreatorLayer::scene();
        auto transitionFade = CCTransitionFade::create(0.5, browserLayer);
        CCDirector::sharedDirector()->pushScene(transitionFade);*/
    }

    /*
     * Helpers
     */
    void showQuestExclamationMark(){
        auto menu = static_cast<CCMenu*>(this->getChildByID("creator-buttons-menu"));
        auto GSM = GameStatsManager::sharedState();

        bool showExclamation = false;
        for(int i = 1; i < 4; i++){
            GJChallengeItem* item = GSM->getChallenge(i);
            if(item != nullptr && item->m_canClaim) showExclamation = true;
        }

        if(menu == nullptr || !showExclamation) return;

        auto questBtn = static_cast<CCMenuItemSpriteExtra*>(menu->getChildByID("quests-button"));
        if(questBtn == nullptr) return;

        auto existingNode = questBtn->getChildByID("bi-exclamation-mark");
        if(existingNode != nullptr){
            existingNode->setVisible(true);
            return;
        }

        auto exMark = CCSprite::createWithSpriteFrameName("exMark_001.png");
        exMark->setPosition({16.5,75});
        exMark->setScale(0.7f);
        exMark->setID("bi-exclamation-mark");
        questBtn->addChild(exMark);
    }

    /*
     * Hooks
     */

    bool init() {
        if(!CreatorLayer::init()) return false;

        //betterinfo btn
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto menu = CCMenu::create();
        menu->setPositionX(winSize.width - 22);
        menu->setID("bi-center-right-menu");

        auto buttonSprite = BetterInfo::createBISprite(Mod::get()->getSettingValue<bool>("alt-button") ? "BI_altBtn_001.png" : "BI_mainButton_001.png");
        buttonSprite->setScale(.9f);
        auto buttonButton = CCMenuItemSpriteExtra::create(
            buttonSprite,
            this,
            menu_selector(BICreatorLayer::onCustomCreatorLayer)
        );
        buttonButton->setSizeMult(1.2f);
        //buttonButton->setPosition({door->getPositionX() - 1,0});
        menu->addChild(buttonButton);
        buttonButton->setID("bi-main-button");

        this->addChild(menu);

        showQuestExclamationMark();

        /*auto alert = CM->updateCompleteDialog();
        if(alert) {
            alert->m_pParent = this;
            alert->show();
        }

        auto alert2 = CM->incompatibleModDialog();
        if(alert2) {
            alert2->m_pParent = this;
            alert2->show();
        }*/

        return true;
    }

    void onChallenge(CCObject* sender) {
        auto child = static_cast<CCNode*>(static_cast<CCNode*>(sender)->getChildByID("bi-exclamation-mark"));
        if(child) {
            child->setVisible(false);
        }

        CreatorLayer::onChallenge(sender);
    }

    void sceneWillResume(){
        CreatorLayer::sceneWillResume();

        showQuestExclamationMark();
    }
};