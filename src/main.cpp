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
    BetterInfoStats::sharedState()->finishLoading();
}

void BI_DLL fixLevelLists() {
    log::info("Deleting level lists created by Mac bug");

    auto LLM = LocalLevelManager::sharedState();
    auto lists = CCArrayExt<GJLevelList*>(LLM->m_localLists);

    std::vector<GJLevelList*> blanks;

    for(auto list : lists) {
        if(list->m_listID == 0 && list->m_listName.empty() && (list->m_levels.size() == 0) && list->m_levelsString.empty()) {
            blanks.push_back(list);
        }
    }

    if(blanks.size() > 10) {
        for(auto list : blanks) {
            LLM->m_localLists->removeObject(list);
        }
    }

    log::info("Deleting level lists semi-favorited by incorrectly expecting 2.200 behavior");
    auto GLM = GameLevelManager::sharedState();

    std::vector<gd::string> toRemove;

    auto lists2 = CCDictionaryExt<gd::string, GJLevelList*>(GLM->m_favoriteLists);
    for(auto [key,value] : lists2) {
        if(!value->m_favorite) toRemove.push_back(key);
    }

    for(auto key : toRemove) {
        GLM->m_favoriteLists->removeObjectForKey(key);
    }
}

class BI_DLL $modify(MenuLayer) {
    bool init() {
        if(!MenuLayer::init()) return false;

        BetterInfo::loadImportantNotices(this);

        static bool managersLoaded = false;
        if(!managersLoaded) {
            managersLoaded = true;
            finishLoadingManagers();
            fixLevelLists();
        }

        return true;
    }
};

$on_mod(Loaded) {
    #ifdef GEODE_IS_ANDROID
        BetterInfo::setAmazon(((GJMoreGamesLayer* volatile)nullptr)->getMoreGamesList()->count() == 0);
    #endif
    Loader::get()->queueInMainThread([] {
        loadManagers();
    });
}