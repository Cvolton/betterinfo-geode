#pragma once

#include "../utils.hpp"
#include <Geode/Geode.hpp>

class BI_DLL BetterInfoCache {
	std::unordered_map<int, GJUserScore*> m_userScoreCache;
public:
	static BetterInfoCache* sharedState();

	void cacheFollowedCreators();
};