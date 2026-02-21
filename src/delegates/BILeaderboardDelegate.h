#pragma once

#include <Geode/Geode.hpp>

class BILeaderboardDelegate {
public:
	virtual void onLeaderboardFinished(cocos2d::CCArray* scores, int stat) = 0;
};