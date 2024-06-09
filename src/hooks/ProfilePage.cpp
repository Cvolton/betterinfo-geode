#include <Geode/Geode.hpp>
#include <Geode/modify/ProfilePage.hpp>

#include "../utils.hpp"
#include "../managers/BetterInfoOnline.h"
#include "../layers/LeaderboardViewLayer.h"
#include "../layers/CreatorInfoPopup.h"

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

class BI_DLL $modify(BIProfilePage, ProfilePage) {
    /*
     * Callbacks
     */

    void onProfilePageInfo(CCObject* sender){
        auto score = this->m_score;
        auto GM = GameManager::sharedState();

        std::ostringstream contentStream;
        if(score->m_userID == GM->m_playerUserID) contentStream << "<cy>User ID:</c> " << score->m_userID
            << "\n<cr>Account ID:</c> " << score->m_accountID
            << "\n\n";//Leaderboard Icon: " << StaticStringHelper::getIconType(score->getIconType()) (is not sent)
        contentStream << "<cg>Friend Requests:</c> " << StaticStringHelper::getFriendRequestType(score->m_friendStatus)
            << "\n<cl>Private Messages:</c> " << StaticStringHelper::getMessageType(score->m_messageState)
            << "\n<cp>Comment History:</c> " << StaticStringHelper::getMessageType(score->m_commentHistoryStatus)
            << "\n";
        if(score->m_userID == GM->m_playerUserID) contentStream << "\n<co>Bootups:</c> " << GM->m_bootups;

        //if(score->m_userID == cvoltonID) contentStream << "\n\nThis user is epic!";

        FLAlertLayer::create("User Info", contentStream.str(), "OK")->show();
    }

    void onProfilePageLeaderboard(CCObject* sender){
        auto score = this->m_score;

        auto scene = LeaderboardViewLayer::scene(score->m_accountID);
        auto transitionFade = CCTransitionFade::create(0.5, scene);
        CCDirector::sharedDirector()->pushScene(transitionFade);
    }

    void onProfilePageCreator(CCObject* sender){
        CreatorInfoPopup::create(m_score->m_userID)->show();
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

    /*
     * Hooks
     */

    void loadPageFromUserInfo(GJUserScore* a2){
        GameLevelManager::sharedState()->storeUserName(a2->m_userID, a2->m_accountID, a2->m_userName);

        ProfilePage::loadPageFromUserInfo(a2);

        auto GM = GameManager::sharedState();
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        if(auto leftMenu = m_mainLayer->getChildByID("left-menu")) {
            auto infoSprite = CCSprite::createWithSpriteFrameName("GJ_infoBtn_001.png");
            infoSprite->setScale(0.7f);
            auto infoButton = CCMenuItemSpriteExtra::create(
                infoSprite,
                this,
                menu_selector(BIProfilePage::onProfilePageInfo)
            );
            infoButton->setID("info-button"_spr);
            leftMenu->addChild(infoButton);
            this->m_buttons->addObject(infoButton);

            leftMenu->updateLayout();
        }

        if(auto playerStats = m_mainLayer->getChildByID("stats-menu")) {
            if(auto creatorIcon = playerStats->getChildByID("creator-points-icon")) {
                m_buttons->removeObject(creatorIcon);
                m_buttons->addObject(BetterInfo::replaceWithButton(creatorIcon, this, menu_selector(BIProfilePage::onProfilePageCreator)));
            }
        }

        if(a2->m_userID != GameManager::sharedState()->m_playerUserID){
            if(auto rankIcon = m_mainLayer->getChildByID("global-rank-icon")) {
                rankIcon->setVisible(false);
            }

            auto leaderboardButtonSprite = CCSprite::createWithSpriteFrameName("GJ_plainBtn_001.png");
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
            leaderboardButton->setID("leaderboard-button"_spr);
            leaderboardButton->setPosition({46, -12});
            m_buttonMenu->addChild(leaderboardButton);
            this->m_buttons->addObject(leaderboardButton);

            auto refreshSprite = CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
            auto refreshButton = CCMenuItemSpriteExtra::create(
                refreshSprite,
                this,
                menu_selector(BIProfilePage::onProfilePageReload)
            );
            refreshButton->setID("refresh-button"_spr);
            m_buttonMenu->addChild(refreshButton);
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
            userIDBtn->setPosition(CCPoint(38  + (userIDNode->getScaledContentSize().width / 2),-254));
            userIDBtn->setID("userid-button"_spr);
            m_buttonMenu->addChild(userIDBtn);
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
            accountIDBtn->setPosition(CCPoint(38  + (accountIDBtn->getScaledContentSize().width / 2),-264));
            accountIDBtn->setID("accountid-button"_spr);
            m_buttonMenu->addChild(accountIDBtn);
            this->m_buttons->addObject(accountIDBtn);
        }

    }

    bool init(int id, bool a2){
        if(!ProfilePage::init(id, a2)) return false;

        if(auto followBtn = m_buttonMenu->getChildByID("follow-button")) {
            followBtn->setPosition({16, -224});
        }

        if(auto followHint = m_mainLayer->getChildByID("follow-hint")) {
            followHint->setVisible(false);
        }

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
};