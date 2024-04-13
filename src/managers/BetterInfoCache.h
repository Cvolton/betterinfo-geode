#pragma once

#include "BaseJsonManager.h"
#include "../delegates/UploadDateDelegate.h"

class BI_DLL BetterInfoCache : public BaseJsonManager {
	std::unordered_set<int> m_attemptedUsernames;
	std::unordered_set<int> m_attemptedLevelDates;
	std::unordered_map<int, int> m_coinCounts;
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

	void checkLevelsFromDict(CCDictionary* dict);
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
};