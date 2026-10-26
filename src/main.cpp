#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/GameManager.hpp>
#include <asp/fs.hpp>

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

        // clean up cache.json v1 (not used anymore)
        async::spawn([] -> arc::Future<void> {;
            if(auto err = asp::fs::remove(Mod::get()->getSaveDir() / "cache.json").err()) {
                log::warn("Failed to delete cache.json: {}", err->message());
            }

            co_return;
        });

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