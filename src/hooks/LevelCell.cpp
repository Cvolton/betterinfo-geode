#include <Geode/Bindings.hpp>
#include <Geode/Modify/LevelCell.hpp>

#include "../layers/UnregisteredProfileLayer.h"
#include "../utils.hpp"

using namespace geode::prelude;

class $modify(LevelCell) {
    void onViewProfile(CCObject* sender) {

        if(this->m_level->m_accountID == 0) {
            UnregisteredProfileLayer::displayProfile(this->m_level->m_userID, this->m_level->m_creatorName);
            return;
        }

        LevelCell::onViewProfile(sender);
    }

    void loadCustomLevelCell() {
        LevelCell::loadCustomLevelCell();

        auto layer = static_cast<CCLayer*>(this->getChildren()->objectAtIndex(1));

        bool menuDone = false;
        for(unsigned int i = 0; i < layer->getChildrenCount(); i++){
            if(this->m_level->m_songID){
                auto bmFont = typeinfo_cast<CCLabelBMFont*>(layer->getChildren()->objectAtIndex(i));
                if(bmFont && bmFont->getPositionX() == 52 && bmFont->getPositionY() == 33 && !BetterInfo::isNewGrounds(this->m_level->m_songID)) bmFont->setColor({249,170,190});
            }


            auto menu = typeinfo_cast<CCMenu*>(layer->getChildren()->objectAtIndex(i));
            if(!menuDone && menu != nullptr){
                auto playerName = static_cast<CCMenuItemSpriteExtra*>(menu->getChildren()->objectAtIndex(1));
                playerName->setEnabled(true);

                std::ostringstream idText;
                idText << "#" << this->m_level->m_levelID;
                auto idTextNode = CCLabelBMFont::create(idText.str().c_str(), "chatFont.fnt");
                idTextNode->setPosition({33,33});
                idTextNode->setAnchorPoint({1,0});
                idTextNode->setScale(0.6f);
                idTextNode->setColor({51,51,51});
                idTextNode->setOpacity(152);
                menu->addChild(idTextNode);
                //TODO: CvoltonManager
                if(this->m_level->m_dailyID > 0){// || CvoltonManager::sharedState()->getOption("white_id")){
                    idTextNode->setColor({255,255,255});
                    idTextNode->setOpacity(200);
                }

                if(this->m_level->m_dailyID > 0){

                    const int maxDaily = 100000;

                    std::ostringstream dailyText;
                    dailyText << ((this->m_level->m_dailyID >= maxDaily) ? "Weekly" : "Daily") << " #" << (this->m_level->m_dailyID % maxDaily);
                    auto dailyTextNode = CCLabelBMFont::create(dailyText.str().c_str(), "chatFont.fnt");
                    dailyTextNode->setPosition({33,43});
                    dailyTextNode->setAnchorPoint({1,0});
                    dailyTextNode->setScale(0.6f);
                    dailyTextNode->setColor({255,255,255});
                    dailyTextNode->setOpacity(200);
                    menu->addChild(dailyTextNode);

                }

                menuDone = true;
            }
        }
    }
};