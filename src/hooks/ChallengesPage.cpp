#include <Geode/Geode.hpp>
#include <Geode/modify/ChallengesPage.hpp>

using namespace geode::prelude;

class $modify(BIChallengesPage, ChallengesPage) {
    CCMenuItemSpriteExtra* m_nextArrowBtn;
    CCMenuItemSpriteExtra* m_prevArrowBtn;
    std::vector<CCNode*> m_queuedNodes;
    std::vector<CCNode*> m_queuedDots;

    /*
     * Callbacks
     */
    void onPrev(CCObject* sender) {
        m_fields->m_prevArrowBtn->setVisible(false);
        m_fields->m_nextArrowBtn->setVisible(true);

        for(intptr_t i = 1; i <= 3; i++) {
            if(m_challengeNodes->objectForKey(i)) {
                static_cast<CCNode*>(m_challengeNodes->objectForKey(i))->setVisible(true);
            }
        }

        for(auto node : m_fields->m_queuedNodes) {
            node->setVisible(false);
        }

        for(auto dot : m_fields->m_queuedDots) {
            dot->setVisible(false);
        }

        intptr_t i = 1;
        for(auto dot : CCArrayExt<CCNode*>(m_dots)) {
            dot->setVisible(GameStatsManager::sharedState()->getQueuedChallenge(i++) != nullptr);
        }
    }

    void onNext(CCObject* sender) {
        m_fields->m_prevArrowBtn->setVisible(true);
        m_fields->m_nextArrowBtn->setVisible(false);

        for(intptr_t i = 1; i <= 3; i++) {
            if(m_challengeNodes->objectForKey(i)) {
                static_cast<CCNode*>(m_challengeNodes->objectForKey(i))->setVisible(false);
            }
        }

        for(auto node : m_fields->m_queuedNodes) {
            node->setVisible(true);
        }

        for(auto dot : m_fields->m_queuedDots) {
            dot->setVisible(GameStatsManager::sharedState()->getChallenge(dot->getTag()) != nullptr);
        }
        
        for(auto dot : CCArrayExt<CCNode*>(m_dots)) {
            dot->setVisible(false);
        }

    }

    void reloadQuests() {
        auto winSize = CCDirector::get()->getWinSize();

        for(auto node : m_fields->m_queuedNodes) {
            node->removeFromParentAndCleanup(true);
        }
        m_fields->m_queuedNodes.clear();

        for(auto dot : m_fields->m_queuedDots) {
            dot->removeFromParentAndCleanup(true);
        }
        m_fields->m_queuedDots.clear();

        for(intptr_t i = 1; i <= 3; i++) {
            auto node = ChallengeNode::create(GameStatsManager::sharedState()->getQueuedChallenge(i), this, false);
            node->setVisible(false);
            node->setPosition({winSize.width / 2, (winSize.height / 2) + 85.f - (80.f * (i - 1))});
            node->setID(Mod::get()->expandSpriteName(fmt::format("queued-quest-{}", i).c_str()));
            m_mainLayer->addChild(node);
            m_fields->m_queuedNodes.push_back(node);

            if(node->m_challengeItem) {
                if(auto background = node->getChildByID("background")) {
                    background->setVisible(false);

                    auto newBackground = CCScale9Sprite::create("GJ_square04.png", {0, 0, 80, 80});
                    newBackground->setContentSize({300, 70});
                    newBackground->setID("background"_spr);
                    node->addChild(newBackground, -1);

                    static_cast<CCLabelBMFont*>(node->getChildByID("progress-label"))->setString("Queued");
                }
            }

            auto dot = CCLabelBMFont::create("...", "chatFont.fnt");
            dot->setVisible(false);
            dot->setOpacity(100);
            dot->setColor({0,0,0});
            dot->setScale(1.4f);
            dot->setPosition({(winSize.width / 2) - 210 + 30, (winSize.height / 2) + 85.f - (80.f * (i - 1))});
            dot->setID(Mod::get()->expandSpriteName(fmt::format("queued-dot-{}", i).c_str()));
            dot->setTag(i);
            m_mainLayer->addChild(dot);
            m_fields->m_queuedDots.push_back(dot);
        }
    }

    /*
     * Hooks
     */

    bool init() {
        if(!ChallengesPage::init()) return false;

        auto winSize = CCDirector::get()->getWinSize();

        auto arrowMenu = CCMenu::create();
        arrowMenu->setID("arrow-menu"_spr);
        m_mainLayer->addChild(arrowMenu);

        auto nextArrow = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
        nextArrow->setFlipX(true);
        m_fields->m_nextArrowBtn = CCMenuItemSpriteExtra::create(
            nextArrow,
            this,
            menu_selector(BIChallengesPage::onNext)
        );
        //m_fields->m_nextArrowBtn->setPosition({(winSize.width / 2) + 185, - 130});
        m_fields->m_nextArrowBtn->setPosition({(winSize.width / 2) - 35, 0});
        m_fields->m_nextArrowBtn->setID("next-button"_spr);
        arrowMenu->addChild(m_fields->m_nextArrowBtn);

        auto prevArrow = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
        m_fields->m_prevArrowBtn = CCMenuItemSpriteExtra::create(
            prevArrow,
            this,
            menu_selector(BIChallengesPage::onPrev)
        );
        //m_fields->m_prevArrowBtn->setPosition({-5, -130});
        m_fields->m_prevArrowBtn->setPosition({- (winSize.width / 2) + 35, 0});
        m_fields->m_prevArrowBtn->setID("prev-button"_spr);
        m_fields->m_prevArrowBtn->setVisible(false);
        arrowMenu->addChild(m_fields->m_prevArrowBtn);

        reloadQuests();

        return true;
    }

    ChallengeNode* createChallengeNode(int number, bool skipAnimation, float animLength, bool isNew) {
        auto node = ChallengesPage::createChallengeNode(number, skipAnimation, animLength, isNew);
        
        reloadQuests();
        
        return node;
    }
};