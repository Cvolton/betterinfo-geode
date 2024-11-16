#include "eclipse.hpp"
#include <Geode/Geode.hpp>

using namespace eclipse;
using namespace geode::prelude;

void createSettingTab(const char* settingID, MenuTab& tab) {
    auto setting = Mod::get()->getSetting(settingID);

    tab.addToggle(Mod::get()->expandSpriteName(settingID).data(), setting->getDisplayName(), [settingID](bool v) {
        Mod::get()->setSettingValue<bool>(settingID, v);
        eclipse::config::set<bool>(Mod::get()->expandSpriteName(settingID).data(), v);
    }).setDescription(setting->getDescription().value_or(""));

    eclipse::config::set<bool>(Mod::get()->expandSpriteName(settingID).data(), Mod::get()->getSettingValue<bool>(settingID));
}

$on_mod(Loaded) {
    Loader::get()->queueInMainThread([] {
        auto tab = MenuTab::find("BetterInfo");

        GEODE_DESKTOP(createSettingTab("auto-submit", tab));
        createSettingTab("show-comment-ids", tab);
        createSettingTab("show-level-ids", tab);
        createSettingTab("white-id", tab);
    });
    
}