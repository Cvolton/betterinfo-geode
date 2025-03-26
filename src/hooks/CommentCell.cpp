#include <Geode/Geode.hpp>
#include <Geode/modify/CommentCell.hpp>

#include "../layers/UnregisteredProfileLayer.h"

using namespace geode::prelude;

class BI_DLL $modify(BICommentCell, CommentCell) {
    /*
     * Callbacks
     */
    void onProfilePage(CCObject* sender){
        this->m_comment->m_userScore->m_userID = this->m_comment->m_userID;
        UnregisteredProfileLayer::create(this->m_comment->m_userScore, BetterInfo::getParentOfType<InfoLayer>(this))->show();
    }

    void onLevelInfoNoLoad(CCObject* sender){
        auto level = GJGameLevel::create();
        level->m_levelID = this->m_comment->m_levelID;

        InfoLayer* infoLayer = InfoLayer::create(level, nullptr, nullptr);
        infoLayer->m_scene = BetterInfo::getParentOfType<InfoLayer>(this);
        infoLayer->show();
    }

    /**
     * Misc
     */
    void showCommentIDsAlert() {
        createQuickPopup(
            Mod::get()->getName().c_str(),
            "Would you like to show comment IDs next to comments?",
            "No",
            "Yes",
            [](FLAlertLayer* me, bool btn2) {
                if(btn2) {
                    Loader::get()->queueInMainThread([]{
                        Mod::get()->setSettingValue<bool>("show-comment-ids", true);
                    });

                    FLAlertLayer::create(Mod::get()->getName().c_str(), "Refresh for the change to take effect.", "OK")->show();
                }
            }
        );
    }

    /*
     * Hooks
     */

    void loadFromComment(GJComment* b) {
        CommentCell::loadFromComment(b);

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        bool smallCommentsMode = this->m_height == 36; //this is how robtop does the check

        /**
         * Add comment ID
         */
        if(!b->m_commentDeleted && Mod::get()->getSettingValue<bool>("show-comment-ids")) {
            auto idText = CCLabelBMFont::create(fmt::format("#{}", b->m_commentID).c_str(), "chatFont.fnt");
            idText->setPosition(smallCommentsMode ? CCPoint(332, 12.5) : CCPoint(329, 21.5));
            idText->setAnchorPoint({1, .5f});
            idText->setScale(smallCommentsMode ? .325f : .425f);
            idText->setOpacity(110);
            idText->setColor({0,0,0});
            idText->setID("comment-id"_spr);
            m_mainLayer->addChild(idText);
        }

        if(auto menu = static_cast<CCMenu*>(m_mainLayer->getChildByID("main-menu"))) {
            /**
             * Add comments InfoLayer button
             */
            if(b->m_hasLevelID) {
                auto commentsSprite = CCSprite::createWithSpriteFrameName("GJ_chatBtn_001.png");
                commentsSprite->setScale(smallCommentsMode ? 0.35f : 0.5f);
                auto commentsButton = CCMenuItemSpriteExtra::create(
                    commentsSprite,
                    this,
                    menu_selector(BICommentCell::onLevelInfoNoLoad)
                );

                CCPoint smallPosition(206 - (winSize.width / 2), 24.5f - (winSize.height / 2));
                CCPoint largePosition(154 - (winSize.width / 2), 60 - (winSize.height / 2));
                commentsButton->setPosition(smallCommentsMode ? smallPosition : largePosition);
                commentsButton->setSizeMult(1.2f);
                commentsButton->setID("comments-button"_spr);

                menu->addChild(commentsButton);
            }

            /**
             * Add username button
            */
            if(auto usernameMenu = menu->getChildByIDRecursive("username-menu")) {
                if(b->m_accountID == 0) if(auto usernameLabel = typeinfo_cast<CCLabelBMFont*>(usernameMenu->getChildByID("username-button"))) {
                    auto content = std::string_view(usernameLabel->getString());
                    if(content == "" || content == "-") {
                        usernameLabel->setString(fmt::format("- (ID: {})", b->m_userID).c_str());
                    }

                    usernameLabel->removeFromParent();

                    auto buttonButton = CCMenuItemSpriteExtra::create(
                        usernameLabel,
                        this,
                        menu_selector(BICommentCell::onProfilePage)
                    );
                    buttonButton->setSizeMult(1.2f);
                    //buttonButton->setPosition(position + CCPoint(usernameLabel->getScaledContentSize().width / 2, 0));
                    buttonButton->setEnabled(true);
                    buttonButton->setID("username-button"); //using vanilla style node ID because this is a replacement for a vanilla feature

                    usernameMenu->addChild(buttonButton);
                    usernameMenu->updateLayout();
                }
            }
        }
    }
};