#pragma once

#include "BaseJsonManager.h"
#include "../delegates/UploadDateDelegate.h"

class BetterInfoCache : public BaseJsonManager, public LevelManagerDelegate {
	std::set<int> m_attemptedUsernames;
	std::set<int> m_attemptedLevelDates;
	inline static BetterInfoCache* m_instance = nullptr;
	BetterInfoCache();

public:
	UploadDateDelegate* m_uploadDateDelegate = nullptr;

	bool init();

	void validateLoadedData();

	static BetterInfoCache* sharedState(){
	    if(m_instance == nullptr){
	        m_instance = new BetterInfoCache;
	        m_instance->init();
	        m_instance->retain();
	    }
	    return m_instance;
	}

	void checkLevelsFromDict(CCDictionary* dict);
	void cacheLevel(GJGameLevel* level);
	void cacheLevels(std::set<int> toDownload);

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

	void loadLevelsFinished(cocos2d::CCArray*, const char*);
    void loadLevelsFailed(const char*);
    void setupPageInfo(std::string, const char*);

	void populateDownloadedSongsFast();
};