#include <Geode/Bindings.hpp>
#include <Geode/Modify/PlayLayer.hpp>

#include "../managers/BetterInfoScheduler.h"
#include "../managers/BetterInfoStats.h"

using namespace geode::prelude;

class $modify(BIPlayLayer, PlayLayer) {

    void levelComplete(){
        PlayLayer::levelComplete();

        auto stats = BetterInfoStats::sharedState();
        stats->logCompletion(this->m_level, this->m_isPracticeMode);
    }

    bool init(GJGameLevel* level){
        if(!PlayLayer::init(level)) return false;

        auto stats = BetterInfoStats::sharedState();
        stats->logPlay(this->m_level);

        return true;
    }

    void onQuit(){
        auto stats = BetterInfoStats::sharedState();
        stats->logPlay(this->m_level);

        if(Mod::get()->getSettingValue<bool>("auto-submit") && this->m_level->m_levelType == GJLevelType::Saved) BetterInfoScheduler::sharedState()->submitLevel(this->m_level);
        PlayLayer::onQuit();
    }

    void resetLevel(){
        PlayLayer::resetLevel();

        auto stats = BetterInfoStats::sharedState();
        stats->logAttempt(this->m_level, this->m_isPracticeMode);
    }
};