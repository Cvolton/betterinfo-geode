#pragma once

#include "BaseJsonManager.h"
#include "../objects/LevelDeath.h"

class BetterInfoStatsV2 : public BaseJsonManager {
	inline static BetterInfoStatsV2* m_instance = nullptr;
	BetterInfoStatsV2();

public:
	bool init();

	void validateLoadedData();

	auto& levelObject(GJGameLevel* level);
	void logDeath(GJGameLevel* level, bool practice, LevelDeath death);
	std::pair<int, int> getCommonFail(GJGameLevel* gjLevel);

	static BetterInfoStatsV2* sharedState(){
	    if(m_instance == nullptr){
	        m_instance = new BetterInfoStatsV2;
	        m_instance->init();
	        m_instance->retain();
	    }
	    return m_instance;
	}
};