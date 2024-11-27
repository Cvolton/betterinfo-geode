#include "qolmod.hpp"

void createSettingTabQOL(const char* settingID, QOLModExt::WindowExt* wnd)
{
    auto setting = Mod::get()->getSetting(settingID);

    auto mod = QOLModExt::createModule(fmt::format("{}{}", ""_spr, setting->getName()));
    mod->setName(setting->getDisplayName());
    mod->setDescription(setting->getDescription().value_or(""));
    mod->setEnabled(Mod::get()->getSettingValue<bool>(settingID));
    mod->setOnToggle([settingID](bool enabled)
    {
        Mod::get()->setSettingValue<bool>(settingID, enabled);
    });

    wnd->addModule(mod);
}

$on_mod(Loaded)
{
    Loader::get()->queueInMainThread([]
    {
        auto window = QOLModExt::createWindow(""_spr);
        window->setName("BetterInfo");
        window->setPriority(701);

        createSettingTabQOL("auto-submit", window);
        createSettingTabQOL("show-comment-ids", window);
        createSettingTabQOL("show-level-ids", window);
        createSettingTabQOL("white-id", window);

        QOLModExt::pushWindow(window);
    });
}