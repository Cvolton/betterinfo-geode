#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include "../managers/BetterInfoScheduler.h"
#include "../managers/BetterInfoStats.h"
#include "../managers/BetterInfoCache.h"

#include "../utils.hpp"

using namespace geode::prelude;

class BI_DLL $modify(BIPlayLayer, PlayLayer) {

    //inline static bool needsReset = false;

    void levelComplete(){
        PlayLayer::levelComplete();

        auto stats = BetterInfoStats::sharedState();
        stats->logCompletion(this->m_level, this->m_isPracticeMode);
    }

    bool init(GJGameLevel* level, bool a, bool b){
        if(!PlayLayer::init(level, a, b)) return false;

        auto stats = BetterInfoStats::sharedState();
        stats->logPlay(this->m_level);

        return true;
    }

    void onQuit(){
        auto stats = BetterInfoStats::sharedState();
        stats->logPlay(this->m_level);

        if(Mod::get()->getSettingValue<bool>("auto-submit") && this->m_level->m_levelType == GJLevelType::Saved) BetterInfoScheduler::sharedState()->submitLevel(this->m_level);
        BetterInfoCache::sharedState()->checkClaimableLists();

        PlayLayer::onQuit();
    }

    void resetLevel(){
        //needsReset = false;

        PlayLayer::resetLevel();

        auto stats = BetterInfoStats::sharedState();
        stats->logAttempt(this->m_level, this->m_isPracticeMode);
    }
};