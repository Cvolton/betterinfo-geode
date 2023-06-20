#include <Geode/Bindings.hpp>
#include <Geode/Modify/LevelPage.hpp>

#include "../layers/ExtendedLevelInfo.h"

using namespace geode::prelude;

class $modify(LevelPage) {
    void onInfo(CCObject* sender) {
        ExtendedLevelInfo::showProgressDialog(this->m_level);
    }
};