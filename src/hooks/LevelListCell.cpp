#include <Geode/Geode.hpp>
#include <Geode/modify/LevelListCell.hpp>

#include "../managers/BetterInfoCache.h"
#include "../layers/UnregisteredProfileLayer.h"
#include "../utils.hpp"

using namespace geode::prelude;

class $modify(BILevelListCell, LevelListCell) {
    /*
     * Hooks
     */

    void loadFromList(GJLevelList* list) {

        LevelListCell::loadFromList(list);

        //TODO: layout for ID node in Node ID mod

        auto idTextNode = CCLabelBMFont::create(fmt::format("#{}", m_levelList->m_listID).c_str(), "chatFont.fnt");
        idTextNode->setPosition({346,m_height - 1});
        idTextNode->setAnchorPoint({1,1});
        idTextNode->setScale(0.6f);
        idTextNode->setColor({51,51,51});
        idTextNode->setOpacity(152);
        idTextNode->setID("list-id-label"_spr);
        if(Mod::get()->getSettingValue<bool>("white-id")){
            idTextNode->setColor({255,255,255});
            idTextNode->setOpacity(200);
        }

        m_mainLayer->addChild(idTextNode);
    }
};