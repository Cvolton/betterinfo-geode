#include <Geode/Bindings.hpp>
#include <Geode/Modify/LevelPage.hpp>

#include "../layers/ExtendedLevelInfo.h"

using namespace geode::prelude;

class $modify(LevelPage) {
    static void onModify(auto& self) {
        auto res = self.setHookPriority("LevelPage::onInfo", 99999);
    }

    void onInfo(CCObject* sender) {
        ExtendedLevelInfo::showProgressDialog(this->m_level);
    }
};