#pragma once

#include "BaseJsonManager.h"
#include "../delegates/UploadDateDelegate.h"
#include <Geode/utils/web.hpp>

class BI_DLL BetterInfoCache : public BaseJsonManager {
	std::unordered_set<int> m_attemptedUsernames;
	std::unordered_set<int> m_attemptedLevelDates;
	std::unordered_map<int, Ref<GJLevelList>> m_updatedCachedLists;
	std::unordered_map<int, Ref<GJLevelList>> m_claimableLists;
	std::unordered_map<int, int> m_coinCounts;
	std::unordered_map<std::string, std::string> m_vaultCodes;
	std::mutex m_coinCountsMutex;
	inline static BetterInfoCache* s_instance = nullptr;
	BetterInfoCache();

public:
	UploadDateDelegate* m_uploadDateDelegate = nullptr;

	bool init();
	void finishLoading();

	void validateLoadedData();
	void establishCachedDicts();

	static BetterInfoCache* sharedState(){
	    if(s_instance == nullptr){
	        s_instance = new BetterInfoCache;
	        s_instance->init();
	    }
	    return s_instance;
	}

	void cacheFollowedCreators();
	void cacheScoresResult(CCArray* scores);
	void cacheScore(GJUserScore* score);

	GJUserScore* getCachedOrPlaceholderScore(int accountID);
	GJUserScore* getCachedScore(int accountID);

	void cacheRatedListsFromMegaResponse(const std::string& megaResponse);
	void cacheRatedLists(int page = 0);
	void cacheList(GJLevelList* list);
	void cacheListAsync(GJLevelList* list);
	void checkClaimableLists();

	void tryShowClaimableListsPopup(CCLayer* scene = nullptr);
	size_t claimableListsCount();
	void downloadClaimableLists();
	void showClaimableLists();
	void removeClaimedLists();

	void checkLevelsFromDict(CCDictionary* dict);
	void markLevelAsFailed(int id);
	bool isLevelFailed(int id);
	void cacheLevel(GJGameLevel* level);
	void cacheLevels(std::set<int> toDownload, SearchType searchType, int levelsPerRequest);

	void storeUserName(int userID, const std::string& username);
	void storeLevelInfo(int levelID, const std::string& field, const std::string& value);
	void storeUploadDate(int levelID, const std::string& date);
	void storeDatesForLevel(GJGameLevel* level);

	std::string getLevelName(int levelID);
	int getDemonDifficulty(int levelID);
	std::string getUserName(int userID, bool download = true);
	int getCoinCount(int levelID);
	std::string getLevelInfo(int levelID, const std::string& field);

	std::string getUploadDate(int levelID, UploadDateDelegate* delegate = nullptr);

	void populateDownloadedSongsFast();

	void cacheVaultCode(const std::string& id, const std::string& code);
	std::string getVaultCode(const std::string& id);
};