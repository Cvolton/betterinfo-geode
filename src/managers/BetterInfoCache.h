#pragma once

#include "../utils.hpp"
#include <Geode/Geode.hpp>

class BI_DLL BetterInfoCache {
	struct CachedLevel {
		std::string m_name;
		int m_coins;
		int m_demonDifficulty;
	};
	friend struct matjson::Serialize<CachedLevel>;

	std::atomic_bool m_loadingStarted = false;

	std::unordered_map<int, Ref<GJUserScore>> m_userScoreCache;
	std::unordered_map<int, CachedLevel> m_levelCache;
	std::unordered_map<int, int> m_levelFailures;
	std::unordered_map<int, time_t> m_levelDateCache;
	std::unordered_map<int, std::string> m_usernameCache;
	std::unordered_map<std::string, std::string> m_vaultCodeCache;

	std::vector<Ref<GJLevelList>> m_levelLists;
	std::vector<Ref<GJLevelList>> m_claimableLists;

	BetterInfoCache();
public:
	static BetterInfoCache* sharedState();

	void startLoading();
	void saveJson();

	arc::Future<> cacheFollowedCreators();
	void cacheUserScore(int userID, GJUserScore* score);
	void cacheUserScores(CCArray* scores);
	GJUserScore* getUserScore(int userID);

	arc::Future<> cacheSavedLevels();
	arc::Future<> cacheLevels(CCDictionary* dict);
	arc::Future<> cacheLevelBatch(std::vector<int> levelIDs, bool rated);
	void cacheLevel(GJGameLevel* level);
	CachedLevel& getLevel(int levelID);

	void fetchLevelDate(int levelID, geode::Function<void(time_t)> callback);

	void fetchUsername(int userID, geode::Function<void(std::string)> callback);
	std::string tryGetUsername(int userID);

	void cacheRatedListsFromMegaResponse(std::string_view response);
	void checkClaimableLists();
	size_t claimableListsCount() const;
	void showClaimableLists();

	void cacheVaultCode(const std::string& id, std::string_view code);
	std::string getVaultCode(const std::string& id);
};