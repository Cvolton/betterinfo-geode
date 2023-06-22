#pragma once

#include "BaseJsonManager.h"

class BetterInfoCache : public BaseJsonManager, public OnlineListDelegate {
	inline static BetterInfoCache* m_instance = nullptr;
	BetterInfoCache();

public:

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

	void checkDailies();
	void cacheLevel(GJGameLevel* level);
	void cacheLevels(std::set<int> toDownload);

	void storeUserName(int userID, std::string username);

	std::string getLevelName(int levelID);
	std::string getUserName(int userID);
	int getCoinCount(int levelID);

	void loadListFinished(cocos2d::CCArray*, const char*);
    void loadListFailed(const char*);
    void setupPageInfo(std::string, const char*);
};