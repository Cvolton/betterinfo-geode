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

        /*Notification::create("important notification")->show();
        BetterInfo::showUnimportantNotification("unimportant 1", NotificationIcon::Success);
        BetterInfo::showUnimportantNotification("unimportant 2", NotificationIcon::Success);
        BetterInfo::showUnimportantNotification("unimportant 3", NotificationIcon::Success);
        BetterInfo::showUnimportantNotification("unimportant 4", NotificationIcon::Success);
        BetterInfo::showUnimportantNotification("unimportant 5", NotificationIcon::Success);
        BetterInfo::showUnimportantNotification("unimportant 6", NotificationIcon::Success);
        Notification::create("important notification 2")->show();*/

        return true;
    }
};

$execute {
    Loader::get()->queueInMainThread([] {
        loadManagers();
    });
}