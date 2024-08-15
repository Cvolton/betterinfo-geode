#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include "../utils.hpp"

using namespace geode::prelude;


class BI_DLL $modify(BIGJBaseGameLayer, GJBaseGameLayer) {
    bool init() {
        if (!GJBaseGameLayer::init()) return false;

        BetterInfo::cancelUnimportantNotifications();

        return true;
    }
};