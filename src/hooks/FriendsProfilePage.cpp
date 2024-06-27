#include <Geode/Geode.hpp>
#include <Geode/modify/FriendsProfilePage.hpp>

#include "../utils.hpp"
#include "../managers/BetterInfoCache.h"

using namespace geode::prelude;

class BI_DLL $modify(BIFriendsProfilePage, FriendsProfilePage) {
    struct Fields {
        Ref<CCMenuItemSpriteExtra> m_refreshBtn;
        bool m_isFollowed = false;
    };

    /*
     * Other
     */
    void friendsInit() {
        if(auto topRightMenu = m_mainLayer->getChildByID("top-right-menu")) {
            auto followedSprite = BasedButtonSprite::create(
                CCSprite::createWithSpriteFrameName("gj_heartOn_001.png"),
                BaseType::Account,
                0,
                1
            );

            auto followedBtn = CCMenuItemSpriteExtra::create(
                followedSprite,
                this,
                menu_selector(BIFriendsProfilePage::onFollowBtn)
            );
            followedBtn->setID("followed-button"_spr);

            topRightMenu->addChild(followedBtn);
            topRightMenu->updateLayout();

        }
    }

    void setFollowList() {
        auto GLM = GameLevelManager::sharedState();
        auto BICache = BetterInfoCache::sharedState();
        
        std::map<std::string, Ref<GJUserScore>> followedCreators;
        
        for(auto [key,trueString] : CCDictionaryExt<gd::string, CCString*>(GLM->m_followedCreators)) {
            auto id = BetterInfo::stoi(key);
            auto score = BICache->getCachedOrPlaceholderScore(id);

            std::string username = score->m_userName;
            BetterInfo::strToLower(username);
            while(followedCreators.contains(username)) {
                username += " ";
            }
            followedCreators.emplace(username, score);
        }

        CCArrayExt<GJUserScore*> followedCreatorsArray;

        for(auto [name, score] : followedCreators) {
            followedCreatorsArray.push_back(score);
        }

        getUserListFinished(followedCreatorsArray.inner(), UserListType::Friends);

        m_fields->m_isFollowed = true;
        m_totalFriends->setString(fmt::format("Total Followed: {}", followedCreatorsArray.size()).c_str());

        GLM->m_userListDelegate = nullptr;
    }

    /*
     * Callbacks
     */
    void onFollowBtn(CCObject* sender) {
        if(m_type != UserListType::Friends) return;

        
        if(auto topRightMenu = m_mainLayer->getChildByID("top-right-menu")) {
            if(auto btn = topRightMenu->getChildByID("followed-button"_spr)) btn->removeFromParent();
            if(auto btn = topRightMenu->getChildByID("blocked-button")) btn->removeFromParent();
        }

        m_fields->m_refreshBtn = m_refreshBtn;
        m_refreshBtn->removeFromParent();

        if(auto label = typeinfo_cast<CCLabelBMFont*>(m_mainLayer->getChildByID("title-label"))) label->setString("Followed");

        setFollowList();
    }

    /*
     * Hooks
     */
    
    bool init(UserListType type){
        if(!FriendsProfilePage::init(type)) return false;

        if(m_type == UserListType::Friends) friendsInit();

        return true;
    }

    void onClose(CCObject* sender) {
        if(m_fields->m_isFollowed) m_type = UserListType::Blocked;
        FriendsProfilePage::onClose(sender);
    }
};