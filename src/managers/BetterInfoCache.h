#pragma once

#include "../utils.hpp"
#include <Geode/Geode.hpp>

class BI_DLL BetterInfoCache {
	struct CachedLevel {
		std::string m_name;
		int m_coins;
		int m_demonDifficulty;
	};

	std::atomic_bool m_loadingStarted = false;

	std::unordered_map<int, Ref<GJUserScore>> m_userScoreCache;
	std::unordered_map<int, CachedLevel> m_levelCache;
	std::unordered_map<int, int> m_levelFailures;

	BetterInfoCache();
public:
	static BetterInfoCache* sharedState();

	void startLoading();

	arc::Future<> cacheFollowedCreators();
	void cacheUserScore(int userID, GJUserScore* score);
	void cacheUserScores(CCArray* scores);
	GJUserScore* getUserScore(int userID);

	arc::Future<> cacheSavedLevels();
	arc::Future<> cacheLevels(CCDictionary* dict);
	arc::Future<> cacheLevelBatch(std::vector<int> levelIDs, bool rated);
	void cacheLevel(GJGameLevel* level);
};