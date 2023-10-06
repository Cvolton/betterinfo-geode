#include <Geode/Bindings.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include "../managers/BetterInfoScheduler.h"
#include "../managers/BetterInfoStats.h"
//#include "../managers/BetterInfoStatsV2.h"
#include "../managers/BetterInfoCache.h"

using namespace geode::prelude;

class $modify(BIPlayLayer, PlayLayer) {

    inline static bool needsReset = false;

    void levelComplete(){
        PlayLayer::levelComplete();

        auto stats = BetterInfoStats::sharedState();
        stats->logCompletion(this->m_level, this->m_isPracticeMode);
    }

    bool init(GJGameLevel* level){
        if(!PlayLayer::init(level)) return false;

        auto stats = BetterInfoStats::sharedState();
        stats->logPlay(this->m_level);

        auto cache = BetterInfoCache::sharedState();
        cache->storeDatesForLevel(this->m_level);

        return true;
    }

    void onQuit(){
        auto stats = BetterInfoStats::sharedState();
        stats->logPlay(this->m_level);

        if(Mod::get()->getSettingValue<bool>("auto-submit") && this->m_level->m_levelType == GJLevelType::Saved) BetterInfoScheduler::sharedState()->submitLevel(this->m_level);
        PlayLayer::onQuit();
    }

    void destroyPlayer(PlayerObject* player, GameObject* spike) {
        PlayLayer::destroyPlayer(player, spike);

        if(!m_isDead || needsReset) return;
        needsReset = true;

        /*log::info("death x: {}", player->m_position.x);
        log::info("death y: {}", player->m_position.y);
        log::info("death rotation: {}", player->getRotation());
        log::info("percent: {}", m_lastDeathPercent);
        BetterInfoStatsV2::sharedState()->logDeath(m_level, m_isPracticeMode, LevelDeath {
            .percentage = m_lastDeathPercent,
            .x = player->m_position.x,
            .y = player->m_position.y,
            .rotation = player->getRotation()
        });*/
    }

    void resetLevel(){
        needsReset = false;

        PlayLayer::resetLevel();

        auto stats = BetterInfoStats::sharedState();
        stats->logAttempt(this->m_level, this->m_isPracticeMode);
    }
};