#include "BetterInfoStatsV2.h"
#include "../utils.hpp"

#include <fstream>
#include <Geode/utils/web.hpp>

bool BetterInfoStatsV2::init(){
    if(!BaseJsonManager::init("stats.json")) return false;
    return true;
}

void BetterInfoStatsV2::validateLoadedData() {
    validateIsObject("levels");
    validateIsObject("levels-daily");
    validateIsObject("levels-gauntlet");
}

BetterInfoStatsV2::BetterInfoStatsV2(){}

auto& BetterInfoStatsV2::levelObject(GJGameLevel* gjLevel) {
    waitForLoad();
    
    const char* key = "levels";
    if(gjLevel->m_dailyID > 0) key = "levels-daily";
    if(gjLevel->m_gauntletLevel) key = "levels-gauntlet";

    auto idStr = std::to_string(gjLevel->m_levelID.value());
    auto& levels = m_json[key];
    if(!levels[idStr].is_object()) levels[idStr] = json::Object();
    auto& level = levels[idStr];
    if(!level["attempts"].is_array()) level["attempts"] = json::Array();
    return level;
}

void BetterInfoStatsV2::logDeath(GJGameLevel* level, bool practice, LevelDeath death) {
    if(practice) return;
    if(level->m_levelType == GJLevelType::Editor) return;

    levelObject(level)["attempts"].as_array().push_back(death);
    doSave();
}

std::pair<int, int> BetterInfoStatsV2::getCommonFail(GJGameLevel* gjLevel) {
    std::unordered_map<int, int> fails;

    auto attempts = levelObject(gjLevel)["attempts"].as_array();
    if(attempts.size() == 0) return {0,0};
    
    for(auto& attempt : attempts) {
        if(!attempt.is_object()) continue;
        if(!attempt["percentage"].is_number()) continue;

        fails[attempt["percentage"].as_int()] += 1;
    }

    auto max = std::max_element(
        fails.begin(), fails.end(),
        [] (const std::pair<int, int>& a, const std::pair<int, int>& b) {
            return a.second < b.second;
        }
    );

    return *max;
}