#include <Geode/Bindings.hpp>
#include <Geode/modify/LevelPage.hpp>

#include "../utils/LevelProgressDialog.h"

using namespace geode::prelude;

class $modify(LevelPage) {
    static void onModify(auto& self) {
        auto res = self.setHookPriority("LevelPage::onInfo", 99999);
    }

    void onInfo(CCObject* sender) {
        LevelProgressDialog::show(this->m_level);
    }
};