#include <Geode/Geode.hpp>
#include <Geode/modify/LevelListLayer.hpp>

#include "../utils.hpp"

#include "../managers/BetterInfoCache.h"
#include "../ui/FolderButton.h"

using namespace geode::prelude;

class BI_DLL 
$modify(BILevelListLayer, LevelListLayer) {
    struct Fields {
        Ref<FolderButton> m_folderButton = nullptr;
    };

    void onClaimReward(CCObject* sender){
        LevelListLayer::onClaimReward(sender);

        BetterInfoCache::sharedState()->removeClaimedLists();
    }

    bool init(GJLevelList* list) {
        if (!LevelListLayer::init(list)) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto folderButton = FolderButton::create([this](int id) {
            m_levelList->m_folder = id;
        });
        folderButton->setDisplayFolder(m_levelList->m_folder);
        folderButton->setIsCreated(m_levelList->m_listType == GJLevelType::Editor);
        folderButton->setZOrder(2);
        folderButton->setContentSize(folderButton->getContentSize() - CCSize(0, 2));

        if(auto rightMenu = this->getChildByID("right-side-menu")) {
            if(m_levelList->m_listType != GJLevelType::Editor && m_levelList->m_favorite) {
                rightMenu->addChild(folderButton);
                rightMenu->updateLayout();
            }

            m_fields->m_folderButton = folderButton;
        }

        if(m_levelList->m_listType == GJLevelType::Editor) {
            m_buttonMenu->addChild(folderButton);
            folderButton->setPosition(m_buttonMenu->convertToNodeSpace({31, 66}));
        }


        return true;
    }

    void onFavorite(CCObject* sender) {
        LevelListLayer::onFavorite(sender);

        if(auto rightMenu = this->getChildByID("right-side-menu")) {
            if(m_levelList->m_favorite) rightMenu->addChild(m_fields->m_folderButton);
            else m_fields->m_folderButton->removeFromParent();

            rightMenu->updateLayout();
        }
    }
};