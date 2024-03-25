#pragma once

#include <Geode/Geode.hpp>
#include <queue>

using namespace geode::prelude;

class BetterInfoScheduler : public cocos2d::CCNode {
	inline static BetterInfoScheduler* s_instance = nullptr;
	BetterInfoScheduler();


public:
	std::queue<Ref<GJGameLevel>> m_levelsToSubmit;

	bool init();
	static BetterInfoScheduler* sharedState(){
	    if(s_instance == nullptr){
	        s_instance = new BetterInfoScheduler;
	        s_instance->init();
	    }
	    return s_instance;
	}

    void submitLevel(GJGameLevel*);
    void onLevelSubmit(float dt);

};