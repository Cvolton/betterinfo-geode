#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/GameManager.hpp>

#include <thread>

//#include "managers/BetterInfoStatsV2.h"
#include "managers/BetterInfoStats.h"
#include "managers/BetterInfoCache.h"
#include "utils.hpp"

using namespace geode::prelude;

void BI_DLL loadManagers() {
    BetterInfoStats::sharedState();

    std::thread([] {
        thread::setName("BI Manager Loader");
        //BetterInfoStatsV2::sharedState();
        BetterInfoCache::sharedState();
    }).detach();
    
}

void BI_DLL finishLoadingManagers() {
    BetterInfoCache::sharedState()->finishLoading();
}

class BI_DLL $modify(MenuLayer) {
    bool init() {
        if(!MenuLayer::init()) return false;

        BetterInfo::loadImportantNotices(this);

        static bool managersLoaded = false;
        if(!managersLoaded) {
            managersLoaded = true;
            finishLoadingManagers();
        }

        return true;
    }
};

$execute {
    Loader::get()->queueInMainThread([] {
        loadManagers();
    });
}