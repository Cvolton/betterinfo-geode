#include <Geode/Bindings.hpp>
#include <Geode/modify/LevelCell.hpp>

#include "../managers/BetterInfoCache.h"
#include "../layers/UnregisteredProfileLayer.h"
#include "../utils.hpp"

using namespace geode::prelude;

class $modify(BILevelCell, LevelCell) {
    int m_hoverCount = 0;
    CCPoint m_lastMousePos = {0,0};
    CCNode* m_levelDesc = nullptr;

    static void onModify(auto& self) {
        auto res = self.setHookPriority("LevelCell::onViewProfile", 99999);
    }

    /*void showDesc() {
        if(m_fields->m_levelDesc) return;

        auto parent = CCSprite::create();
        parent->setID("leveldesc"_spr);

        / *auto bg = CCScale9Sprite::create("square02b_small.png", { 0, 0, 40, 40 });
        bg->setColor({ 0, 0, 0 });
        parent->addChild(bg);

        auto label = CCLabelBMFont::create(m_level->m_levelDesc.c_str(), "bigFont.fnt");
        label->setScale(.6f);
        parent->addChild(label);/

        cocos2d::extension::CCScale9Sprite* descBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f });
        descBg->setContentSize({340,55});
        descBg->setColor({130,64,33});
        descBg->setColor({0,0,0});
        descBg->setOpacity(230);
        parent->addChild(descBg, -1);
        descBg->setPosition({0,52});

        auto descText = BetterInfo::fixNullByteCrash(BetterInfo::fixColorCrashes(m_level->getUnpackedLevelDescription()));
        size_t descLength = descText.length();
        float descDelimiter = 1;
        if(descLength > 140) descLength = 140;
        if(descLength > 70) descDelimiter = ((((140 - descLength) / 140) * 0.3f) + 0.7f);
        auto description = TextArea::create(descText, "chatFont.fnt", 1, 295 / descDelimiter, {0.5f,0.5f}, 20, false);
        description->setScale(descDelimiter);
        description->setAnchorPoint({1,1});
        description->setPosition( ( (description->getContentSize() / 2 ) * descDelimiter ) + (CCPoint(340,55) / 2) );
        auto descSprite = CCSprite::create();
        descSprite->addChild(description);
        descSprite->setContentSize({340,55});
        descSprite->setPosition({0,52});
        parent->addChild(descSprite);

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        CCScene::get()->addChild(parent);
        parent->setPositionX(winSize.width / 2);
        m_fields->m_levelDesc = parent;

        description->setOpacity(0);
        description->runAction(CCFadeTo::create(0.3f, 255));
        description->setID("leveldesc-area"_spr);

        descBg->setOpacity(0);
        descBg->runAction(CCFadeTo::create(0.3f, 230));
        descBg->setID("leveldesc-bg"_spr);
    }

    void hideDesc() {
        if(!m_fields->m_levelDesc) return;

        auto description = static_cast<TextArea*>(m_fields->m_levelDesc->getChildByIDRecursive("leveldesc-area"_spr));
        auto descBg = static_cast<CCScale9Sprite*>(m_fields->m_levelDesc->getChildByIDRecursive("leveldesc-bg"_spr));

        //m_fields->m_levelDesc->removeFromParentAndCleanup(true);
        if(description) description->runAction(CCFadeTo::create(0.3f, 0));
        if(descBg) descBg->runAction(CCFadeTo::create(0.3f, 0));

        m_fields->m_levelDesc = nullptr;
    }

    void checkHover(float dt) {
        if(getMousePos() == m_lastMousePos && BetterInfo::isHoveringNode(this) && typeinfo_cast<CCTransitionScene*>(CCScene::get()) == nullptr) {
            m_hoverCount++;

            if(m_hoverCount == 5) {
                log::info("Hovering: {}", m_level->m_levelName);
                showDesc();
            }
        } else {
            m_hoverCount = 0;
            hideDesc();
        };

        m_lastMousePos = getMousePos();
    }*/

    /*
     * Hooks
     */

    void onViewProfile(CCObject* sender) {

        if(this->m_level->m_accountID == 0) {
            UnregisteredProfileLayer::displayProfile(this->m_level->m_userID, this->m_level->m_creatorName);
            return;
        }

        LevelCell::onViewProfile(sender);
    }

    void loadCustomLevelCell() {
        if(std::string(m_level->m_creatorName).empty()) {
            m_level->m_creatorName = BetterInfoCache::sharedState()->getUserName(m_level->m_userID, false);
        }

        LevelCell::loadCustomLevelCell();

        //this->getScheduler()->scheduleSelector(schedule_selector(BILevelCell::checkHover), this, 0.1f, false);

        auto idTextNode = CCLabelBMFont::create(fmt::format("#{}", m_level->m_levelID.value()).c_str(), "chatFont.fnt");
        idTextNode->setPosition({346,79 - (this->m_level->m_dailyID == 0 && Loader::get()->isModLoaded("n.level_pronouns") ? 9.f : 0.f)});
        idTextNode->setAnchorPoint({1,0});
        idTextNode->setScale(0.6f);
        idTextNode->setColor({51,51,51});
        idTextNode->setOpacity(152);
        idTextNode->setID("level-id-label"_spr);
        m_mainLayer->addChild(idTextNode);
        if(this->m_level->m_dailyID > 0 || Mod::get()->getSettingValue<bool>("white-id")){
            idTextNode->setColor({255,255,255});
            idTextNode->setOpacity(200);
        }

        if(this->m_level->m_dailyID > 0){
            idTextNode->setPosition(363,79);

            const int maxDaily = 100000;

            std::ostringstream dailyText;
            dailyText << ((this->m_level->m_dailyID >= maxDaily) ? "Weekly" : "Daily") << " #" << (this->m_level->m_dailyID % maxDaily);
            auto dailyTextNode = CCLabelBMFont::create(dailyText.str().c_str(), "chatFont.fnt");
            dailyTextNode->setPosition({363,89});
            dailyTextNode->setAnchorPoint({1,0});
            dailyTextNode->setScale(0.6f);
            dailyTextNode->setColor({255,255,255});
            dailyTextNode->setOpacity(200);
            dailyTextNode->setID("daily-id-label"_spr);
            m_mainLayer->addChild(dailyTextNode);

            BetterInfoCache::sharedState()->cacheLevel(m_level);
        }

        bool menuDone = false;
        for(unsigned int i = 0; i < m_mainLayer->getChildrenCount(); i++){
            if(this->m_level->m_songID){
                auto bmFont = typeinfo_cast<CCLabelBMFont*>(m_mainLayer->getChildren()->objectAtIndex(i));
                if(bmFont && bmFont->getPositionX() == 52 && bmFont->getPositionY() == 33 && !BetterInfo::isNewGrounds(this->m_level->m_songID)) bmFont->setColor({249,170,190});
            }

            auto menu = typeinfo_cast<CCMenu*>(m_mainLayer->getChildren()->objectAtIndex(i));
            if(!menuDone && menu != nullptr){
                auto playerName = static_cast<CCMenuItemSpriteExtra*>(menu->getChildren()->objectAtIndex(1));
                playerName->setEnabled(true);

                menuDone = true;
            }
        }
    }
};