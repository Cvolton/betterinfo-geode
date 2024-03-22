#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace ServerUtils {
    std::string getBaseURL();
    void getOnlineLevels(GJSearchObject* searchObject, std::function<void(std::vector<Ref<GJGameLevel>>, bool)> callback, bool cacheLevels = true);
    cocos2d::CCArray* getStoredOnlineLevels(const std::string& key);
    void resetCache();
    bool showCFError(const std::string& data);
}