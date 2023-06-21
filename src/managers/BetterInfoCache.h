#pragma once
#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class BetterInfoCache : public CCObject, public OnlineListDelegate {
	inline static BetterInfoCache* m_instance = nullptr;
	BetterInfoCache();


public:
	json::Value m_json = json::Object();
	std::unordered_map<int, std::string> m_levelNameDict;
	std::unordered_map<int, int> m_coinCountDict;

	bool init();
	Result<> load();
	Result<> save();
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

	const char* getLevelName(int levelID);
	int getCoinCount(int levelID);

	void loadListFinished(cocos2d::CCArray*, const char*);
    void loadListFailed(const char*);
    void setupPageInfo(std::string, const char*);
};