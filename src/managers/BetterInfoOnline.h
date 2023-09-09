#pragma once

#include <Geode/Geode.hpp>
#include "../delegates/BILeaderboardDelegate.h"

using namespace geode::prelude;

class BetterInfoOnline : public cocos2d::CCObject {
	inline static BetterInfoOnline* m_instance = nullptr;
	BetterInfoOnline();


public:
	std::map<int, cocos2d::CCArray*> m_scoreDict;
	BILeaderboardDelegate* m_scoreDelegate = nullptr;
	ProfilePage* m_scoreProfilePage = nullptr;

	bool init();
	static BetterInfoOnline* sharedState(){
	    if(m_instance == nullptr){
	        m_instance = new BetterInfoOnline;
	        m_instance->init();
	        m_instance->retain();
	    }
	    return m_instance;
	}

    void loadScores(int accountID, bool force);
    void loadScores(int accountID, bool force, BILeaderboardDelegate* delegate);
    void loadScores(int accountID, bool force, ProfilePage* profilePage);
    void generateScores(const std::string& response, int accountID);
    void sendScores(cocos2d::CCArray* scores, int accountID);
    void sendScoreToProfilePage(cocos2d::CCArray* scores, int accountID);
};