#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

#include <thread>

//#include "managers/BetterInfoStatsV2.h"
#include "managers/BetterInfoStats.h"
#include "managers/BetterInfoCache.h"
#include "utils.hpp"

using namespace geode::prelude;

void loadManagers() {
    BetterInfoStats::sharedState();

    std::thread([] {
        //BetterInfoStatsV2::sharedState();
        BetterInfoCache::sharedState();
    }).detach();

    Mod::get()->setSavedValue<std::string>("last_launch_version", Mod::get()->getVersion().toString());
}

class $modify(MenuLayer) {
    bool init() {
        if(!MenuLayer::init()) return false;

        loadManagers();
        BetterInfo::loadImportantNotices(this);

        return true;
    }
};