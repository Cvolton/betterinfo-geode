#pragma once

#include <Geode/Geode.hpp>
#include "../delegates/BILeaderboardDelegate.h"
#include "../utils.hpp"

using namespace geode::prelude;

class BI_DLL BetterInfoOnline : public cocos2d::CCObject {
	inline static BetterInfoOnline* s_instance = nullptr;
	BetterInfoOnline();


public:
	std::map<int, Ref<cocos2d::CCArray>> m_scoreDict;
	std::set<BILeaderboardDelegate*> m_delegates;

	bool init();
	static BetterInfoOnline* sharedState(){
	    if(s_instance == nullptr){
	        s_instance = new BetterInfoOnline;
	        s_instance->init();
	    }
	    return s_instance;
	}

    void loadScores(int accountID, bool force, BILeaderboardDelegate* delegate, Ref<ProfilePage> profilePage, int stat = 0);
    void loadScores(int accountID, bool force, BILeaderboardDelegate* delegate);
    void loadScores(int accountID, bool force, Ref<ProfilePage> profilePage);
    void generateScores(const std::string& response, int accountID);
    void sendScores(cocos2d::CCArray* scores, int accountID, BILeaderboardDelegate* delegate, Ref<ProfilePage> profilePage);
    void sendScoreToProfilePage(cocos2d::CCArray* scores, int accountID, Ref<ProfilePage> profilePage);
	void cancelScoresRequest(BILeaderboardDelegate* delegate);
};