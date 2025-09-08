#include <Geode/Geode.hpp>
//#include <Geode/modify/CustomListView.hpp>

#include "../utils.hpp"

using namespace geode::prelude;

static bool s_lastRequestSent = false;

/*class BI_DLL $modify(CustomListView) {
	static CustomListView* create(cocos2d::CCArray* a, TableViewCellDelegate* b, float c, float d, int e, BoomListType f, float g) {
		if(f == BoomListType::User) f = BoomListType::Request;

		return CustomListView::create(a, b, c, d, e, f, g);
	}
};*/

#include <Geode/modify/FRequestProfilePage.hpp>
class BI_DLL $modify(FRequestProfilePage) {
	void setupCommentsBrowser(cocos2d::CCArray* requests) {
		static_assert(&FRequestProfilePage::setupCommentsBrowser, "Hook not implemented");

		if(m_listLayer) {
			auto director = cocos2d::CCDirector::sharedDirector();
			auto touchDispatcher = director->getTouchDispatcher();
			touchDispatcher->unregisterForcePrio(m_listLayer);
			m_listLayer->removeFromParentAndCleanup(true);
			m_listLayer = nullptr;
		}

		auto director = cocos2d::CCDirector::sharedDirector();
		auto winSize = director->getWinSize();

		auto listView = CustomListView::create(requests, nullptr, 196.0f, 340.0f, 0, BoomListType::Request, 0.0f);
		auto commentListLayer = GJCommentListLayer::create(listView, "Friend Requests", {0xBF, 0x72, 0x3E, 0xFF}, 340.0f, 196.0f, 0);
		m_listLayer = commentListLayer;

		auto position = cocos2d::CCPoint((winSize.width - 340.0f) * 0.5f, ((winSize.height - 196.0f) * 0.5f) - 10.0f + 5.0f);
		commentListLayer->setPosition(position);

		m_mainLayer->addChild(commentListLayer);

		if(auto list = commentListLayer->m_list) {
			m_buttonMenu->setTouchPriority(list->getTouchPriority() - 2);
		}
	}
};

#include <Geode/modify/FriendRequestPopup.hpp>
class BI_DLL $modify(BIFriendRequestPopup, FriendRequestPopup) {
	struct Fields {
		bool m_sent = false;
	};

	/**
	 * Callbacks
	 */
	void onCancel(cocos2d::CCObject* sender) {
		if(!m_fields->m_sent) return;

		auto GLM = GameLevelManager::sharedState();
		GLM->m_uploadActionDelegate = this;
		if(GLM->deleteFriendRequests(m_request->m_accountID, nullptr, true)) {
			m_popup = UploadActionPopup::create(this, "Removing Friend Request...");
			m_popup->m_delegate = this;
			m_popup->show();
		}
	}

	/**
	 * Hooks
	 */

	void onClose(cocos2d::CCObject* sender) {
		Ref request = m_request;
		auto id = request->m_requestID;
		if(m_fields->m_sent) request->m_requestID = 0;

		FriendRequestPopup::onClose(sender);

		request->m_requestID = id;
	}

	bool init(GJFriendRequest* request) {
		m_fields->m_sent = s_lastRequestSent;
		s_lastRequestSent = false;

		if(!FriendRequestPopup::init(request)) return false;

		// friend request id
		auto idText = CCLabelBMFont::create(fmt::format("#{}", request->m_requestID).c_str(), "chatFont.fnt");
		idText->setPosition({446, 235});
		idText->setAnchorPoint({1, .5f});
		idText->setScale(.35f);
		idText->setOpacity(200);
		idText->setColor({0,0,0});
		idText->setID("request-id"_spr);
		m_mainLayer->addChild(idText);

		// cancel button in sent requests

		if(m_fields->m_sent) {
			if(auto menu = m_mainLayer->getChildByID("bottom-menu")) {
				if(auto acceptBtn = menu->getChildByID("accept-button")) acceptBtn->removeFromParent();
				if(auto removeBtn = menu->getChildByID("remove-button")) removeBtn->removeFromParent();
				if(auto blockBtn = menu->getChildByID("block-button")) blockBtn->removeFromParent();

				auto removeBtn = CCMenuItemSpriteExtra::create(
					ButtonSprite::create(
						"Cancel",
						65,
						0,
						0.55f,
						true,
						"bigFont.fnt",
						"GJ_button_04.png",
						26
					),
					this,
					menu_selector(BIFriendRequestPopup::onCancel)
				);
				removeBtn->setID("remove-button"); //imitating a vanilla node

				menu->addChild(removeBtn);
				menu->updateLayout();
			}

			if(auto username = typeinfo_cast<CCLabelBMFont*>(m_mainLayer->getChildByID("username-label"))) {
				std::string text = username->getString();
				if(text.length() > 5) {
					text = "To:" + text.substr(5);
					username->setString(text.c_str());
				}
			}
		}

		return true;
	}
};

#include <Geode/modify/GJRequestCell.hpp>
class BI_DLL $modify(GJRequestCell) {
	static void onModify(auto& self) {
        (void) self.setHookPriority("GJRequestCell::FLAlert_Clicked", Priority::Last);
        (void) self.setHookPriority("GJRequestCell::markAsRead", Priority::Last);
    }

	/**
	 * Helpers
	 */
	bool isParentSent() {
		CCNode* parent = this;
		while(parent) {
			parent = parent->getParent();
			auto page = typeinfo_cast<FRequestProfilePage*>(parent);
			if(page) {
				return page->m_sent;
			}
		}

		return false;
	}
	/**
	 * Hooks
	 */

	void FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
		if(!isParentSent()) return GJRequestCell::FLAlert_Clicked(layer, btn2);

		if(btn2) {
			auto GLM = GameLevelManager::sharedState();
			GLM->m_uploadActionDelegate = this;
			if(GLM->deleteFriendRequests(m_score->m_accountID, nullptr, true)) {
				m_popup = UploadActionPopup::create(this, "Removing Friend Request...");
				m_popup->m_delegate = this;
				m_popup->show();
			}
		}
	}

	void markAsRead() {
		if(!isParentSent()) return GJRequestCell::markAsRead();
	}

	void onViewFriendRequest(cocos2d::CCObject* sender) {
		s_lastRequestSent = isParentSent();

		GJRequestCell::onViewFriendRequest(sender);
	}
};