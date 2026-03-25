#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/GameManager.hpp>

#include "managers/BetterInfoStats.h"
#include "managers/BetterInfoCache.h"
#include "utils.hpp"

using namespace geode::prelude;

void BI_DLL loadManagers() {
    BetterInfoStats::sharedState();
    BetterInfoCache::sharedState();
    
}

void BI_DLL finishLoadingManagers() {
    BetterInfoCache::sharedState()->startLoading();
    BetterInfoStats::sharedState()->finishLoading();
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

$on_mod(Loaded) {
    Loader::get()->queueInMainThread([] {
        loadManagers();

        if(Mod::get()->getSettingValue<bool>("reset-saved-filters")) {
            BetterInfo::clearSavedValueGroup("user_search_");
        }
    });
}