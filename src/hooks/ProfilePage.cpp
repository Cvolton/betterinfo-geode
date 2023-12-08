#include <Geode/Bindings.hpp>
#include <Geode/modify/ProfilePage.hpp>

#include "../utils.hpp"
#include "../managers/BetterInfoOnline.h"
#include "../layers/LeaderboardViewLayer.h"

#include "GJUserScore.hpp"

using namespace geode::prelude;

namespace StaticStringHelper {
    std::string getFriendRequestType(int type){
        switch(type){
            case 0: return "Enabled";
            case 1: return "Disabled";
            default: return "Unknown";
        }
    }

    std::string getMessageType(int type){
        switch(type){
            case 0: return "Enabled";
            case 1: return "Friends Only";
            case 2: return "Disabled";
            default: return "Unknown";
        }
    }

    std::string getIconType(int type){
        switch(type){
            case 0: return "Cube";
            case 1: return "Ship";
            case 2: return "Ball";
            case 3: return "UFO";
            case 4: return "Wave";
            case 5: return "Robot";
            case 6: return "Spider";
            case 7: return "Swingcopter";
            default: return "None";
        }
    }
};

class $modify(BIProfilePage, ProfilePage) {
    /*
     * Callbacks
     */

    void onProfilePageInfo(CCObject* sender){
        auto score = this->m_score;
        auto GM = GameManager::sharedState();

        std::ostringstream contentStream;
        if(score->m_userID == GM->m_playerUserID) contentStream << "User ID: " << score->m_userID
            << "\nAccount ID: " << score->m_accountID
            << "\n\n";//Leaderboard Icon: " << StaticStringHelper::getIconType(score->getIconType()) (is not sent)
        contentStream << "Friend Requests: " << StaticStringHelper::getFriendRequestType(score->m_friendStatus)
            << "\nPrivate Messages: " << StaticStringHelper::getMessageType(score->m_messageState)
            << "\nComment History: " << StaticStringHelper::getMessageType(score->m_commentHistoryStatus)
            << "\n";
        if(score->m_userID == GM->m_playerUserID) contentStream << "\nBootups: " << GM->m_bootups;
        else contentStream 
            << "\nGlow Color: #" << static_cast<BIGJUserScore*>(score)->m_fields->m_color3
            << "\nMoons: " << static_cast<BIGJUserScore*>(score)->m_fields->m_moons
            << "\nSwingcopter: #" << static_cast<BIGJUserScore*>(score)->m_fields->m_accSwing
            << "\nJetpack: #" << static_cast<BIGJUserScore*>(score)->m_fields->m_accJetpack;

        //if(score->m_userID == cvoltonID) contentStream << "\n\nThis user is epic!";

        FLAlertLayer::create("User Info", contentStream.str(), "OK")->show();
    }

    void onProfilePageLeaderboard(CCObject* sender){
        auto score = this->m_score;

        auto scene = LeaderboardViewLayer::scene(score->m_accountID);
        auto transitionFade = CCTransitionFade::create(0.5, scene);
        CCDirector::sharedDirector()->pushScene(transitionFade);
    }

    void onProfilePageReload(CCObject* sender){
        auto GLM = GameLevelManager::sharedState();

        auto score = this->m_score;
        GLM->resetStoredUserInfo(score->m_accountID);
        GLM->resetAccountComments(score->m_accountID);

        GLM->getGJUserInfo(this->m_accountID);
    }

    void onProfilePageCopyUserID(CCObject* sender){
        BetterInfo::copyToClipboard(std::to_string(this->m_score->m_userID).c_str(), this);
    }

    void onProfilePageCopyAccountID(CCObject* sender){
        BetterInfo::copyToClipboard(std::to_string(this->m_score->m_accountID).c_str(), this);
    }

    void onProfilePageCopyPlayerName(CCObject* sender){
        BetterInfo::copyToClipboard(this->m_score->getPlayerName().c_str(), this);
    }

    /*
     * Helper funcs
     */

    inline void fixProfilePagePositions(){
        auto layer = static_cast<CCLayer*>(this->getChildren()->objectAtIndex(0));
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        for(unsigned int i = 0; i < this->m_buttonMenu->getChildrenCount(); i++){
            CCNode* node = typeinfo_cast<CCNode*>(this->m_buttonMenu->getChildren()->objectAtIndex(i));
            if(node != nullptr && node->getPositionX() == 12 && node->getPositionY() == -258) node->setPosition(16, -224); //node->setPosition({52, -258}); // //
        }

        CCNode* followTxt = typeinfo_cast<CCNode*>(layer->getChildren()->objectAtIndex(6));
        if(followTxt->getPositionY() == (winSize.height / 2) - 125) followTxt->setVisible(false); //followTxt->setPositionX(followTxt->getPositionX() + 40);
    }

    /*
     * Hooks
     */

    void loadPageFromUserInfo(GJUserScore* a2){
        //TODO: fix mod badge positioning for empty profiles
        GameLevelManager::sharedState()->storeUserName(a2->m_userID, a2->m_accountID, a2->m_userName);

        ProfilePage::loadPageFromUserInfo(a2);

        //fixProfilePagePositions(this);

        auto layer = static_cast<CCLayer*>(this->getChildren()->objectAtIndex(0));
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto menu = this->m_buttonMenu;

        auto infoSprite = CCSprite::createWithSpriteFrameName("GJ_infoBtn_001.png");
        infoSprite->setScale(0.7f);
        auto infoButton = CCMenuItemSpriteExtra::create(
            infoSprite,
            this,
            menu_selector(BIProfilePage::onProfilePageInfo)
        );
        menu->addChild(infoButton);
        infoButton->setPosition({16, -135});
        infoButton->setSizeMult(1.2f);
        infoButton->setID("bi-info-button");

        this->m_buttons->addObject(infoButton);

        if(a2->m_userID != GameManager::sharedState()->m_playerUserID){
            CCNode* usernameLabel = nullptr;
            CCNode* modBadge = nullptr;

            std::vector<CCNode*> possiblyModBadge;

            for(unsigned int i = 0; i < layer->getChildrenCount(); i++){
                CCNode* node = typeinfo_cast<CCNode*>(layer->getChildren()->objectAtIndex(i));
                if(node != nullptr && node->getPositionX() == (winSize.width / 2) - 164 && node->getPositionY() == (winSize.height / 2) + 123) node->setVisible(false); //rank icon
                if(node != nullptr && node->getPositionX() == (winSize.width / 2) && node->getPositionY() == (winSize.height / 2) + 125) { //username label
                    usernameLabel = node;
                    node->setVisible(false); 
                }
                if(node != nullptr && node->getPositionY() == (winSize.height / 2) + 124) possiblyModBadge.push_back(node); //possibly mod badge
            }

            if(usernameLabel) for(auto& badge : possiblyModBadge) {
                if( (usernameLabel->getPositionX() - (usernameLabel->getScaledContentSize().width / 2) - 16) == badge->getPositionX() ) modBadge = badge;
            }

            auto leaderboardButtonSprite = BetterInfo::createBISprite("BI_blankBtn_001.png");
            auto leaderboardSprite = CCSprite::createWithSpriteFrameName(BetterInfo::rankIcon(a2->m_globalRank));
            leaderboardSprite->setZOrder(1);
            leaderboardSprite->setScale(1 / 0.6f);
            if(a2->m_globalRank > 0) {
                if(a2->m_globalRank <= 10) leaderboardSprite->setScale(1.1f);
                else if(a2->m_globalRank <= 50) leaderboardSprite->setScale(1.25f);
                else if(a2->m_globalRank <= 200) leaderboardSprite->setScale(1.3f);
                else if(a2->m_globalRank <= 1000) leaderboardSprite->setScale(1 / 0.7f);
            }
            leaderboardSprite->setPosition({22.5f, 23});
            leaderboardButtonSprite->addChild(leaderboardSprite);
            leaderboardButtonSprite->setScale( (a2->m_globalRank <= 1000 && a2->m_globalRank > 0) ? 0.7f : 0.6f);
            auto leaderboardButton = CCMenuItemSpriteExtra::create(
                leaderboardButtonSprite,
                this,
                menu_selector(BIProfilePage::onProfilePageLeaderboard)
            );
            leaderboardButton->setID("bi-leaderboard-button");
            menu->addChild(leaderboardButton);
            leaderboardButton->setPosition({46, -12});
            leaderboardButton->setSizeMult(1.2f);
            this->m_buttons->addObject(leaderboardButton);

            auto refreshSprite = CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
            auto refreshButton = CCMenuItemSpriteExtra::create(
                refreshSprite,
                this,
                menu_selector(BIProfilePage::onProfilePageReload)
            );
            refreshButton->setID("bi-refresh-button");
            menu->addChild(refreshButton);
            refreshButton->setPosition({0, -269});
            refreshButton->setSizeMult(1.2f);

            this->m_buttons->addObject(refreshButton);

            auto userIDNode = CCLabelBMFont::create(CCString::createWithFormat("User ID: %i", a2->m_userID)->getCString(), "chatFont.fnt");
            userIDNode->setScale(0.6f);
            userIDNode->setColor({255,255,255});
            userIDNode->setOpacity(220);
            auto userIDBtn = CCMenuItemSpriteExtra::create(
                userIDNode,
                this,
                menu_selector(BIProfilePage::onProfilePageCopyUserID)
            );
            userIDBtn->setPosition({38,-248});
            userIDBtn->setAnchorPoint({0,1});
            userIDBtn->setID("bi-userid-button");
            menu->addChild(userIDBtn);
            this->m_buttons->addObject(userIDBtn);

            auto accountIDNode = CCLabelBMFont::create(CCString::createWithFormat("Account ID: %i", a2->m_accountID)->getCString(), "chatFont.fnt");
            accountIDNode->setScale(0.6f);
            accountIDNode->setColor({255,255,255});
            accountIDNode->setOpacity(220);
            auto accountIDBtn = CCMenuItemSpriteExtra::create(
                accountIDNode,
                this,
                menu_selector(BIProfilePage::onProfilePageCopyAccountID)
            );
            accountIDBtn->setPosition({38,-258});
            accountIDBtn->setAnchorPoint({0,1});
            accountIDBtn->setID("bi-accountid-button");
            menu->addChild(accountIDBtn);
            this->m_buttons->addObject(accountIDBtn);

            auto usernameNode = CCLabelBMFont::create(a2->m_userName.c_str(), "bigFont.fnt");
            usernameNode->limitLabelWidth(a2->m_modBadge > 0 ? 140.f : 185.0f, 0.9f, 0.0f);
            auto usernameBtn = CCMenuItemSpriteExtra::create(
                usernameNode,
                this,
                menu_selector(BIProfilePage::onProfilePageCopyPlayerName)
            );
            usernameBtn->setPosition({210,-10});
            usernameBtn->setID("bi-username-button");
            menu->addChild(usernameBtn);
            this->m_buttons->addObject(usernameBtn);

            if(modBadge) modBadge->setPositionX((winSize.width / 2) - (usernameNode->getScaledContentSize().width / 2) - 16);
        }

    }

    bool init(int id, bool a2){
        if(!ProfilePage::init(id, a2)) return false;

        fixProfilePagePositions();

        return true;
    }

    void getUserInfoFailed(int id){
        ProfilePage::getUserInfoFailed(id);

        BetterInfoOnline::sharedState()->loadScores(id, false, this);
    }

    void onClose(CCObject* sender) {
        BetterInfoOnline::sharedState()->m_scoreProfilePage = nullptr;

        ProfilePage::onClose(sender);
    }

    void keyBackClicked() {
        BetterInfoOnline::sharedState()->m_scoreProfilePage = nullptr;

        ProfilePage::keyBackClicked();
    }
};