#include <Geode/Geode.hpp>
//#include <Geode/modify/CustomListView.hpp>

#include "../utils.hpp"

using namespace geode::prelude;

/*class BI_DLL $modify(CustomListView) {
	static CustomListView* create(cocos2d::CCArray* a, TableViewCellDelegate* b, float c, float d, int e, BoomListType f, float g) {
		if(f == BoomListType::User) f = BoomListType::Request;

		return CustomListView::create(a, b, c, d, e, f, g);
	}
};*/

#include <Geode/modify/FRequestProfilePage.hpp>
class BI_DLL $modify(FRequestProfilePage) {
	void setupCommentsBrowser(cocos2d::CCArray* requests) {
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
class BI_DLL $modify(FriendRequestPopup) {
	void onClose(cocos2d::CCObject* sender) {
		Ref request = m_request;
		auto id = request->m_requestID;
		request->m_requestID = 0;

		FriendRequestPopup::onClose(sender);

		request->m_requestID = id;
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
};