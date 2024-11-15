#include <Geode/Geode.hpp>
#include <Geode/modify/LevelPage.hpp>

#include "../utils.hpp"

using namespace geode::prelude;

class BI_DLL $modify(LevelPage) {
    static void onModify(auto& self) {
        auto res = self.setHookPriority("LevelPage::onInfo", Priority::VeryLatePre);
    }

    void onInfo(CCObject* sender) {
        static_assert(&LevelPage::onInfo, "Hook not implemented");

        LevelProgressDialog::show(this->m_level);
    }
};