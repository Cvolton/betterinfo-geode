#pragma once

#include <Geode/Geode.hpp>
#include "../utils.hpp"

using namespace geode::prelude;

namespace ServerUtils {
    BI_DLL std::string getBaseURL();
    BI_DLL std::string getBasePostString();
    BI_DLL void getOnlineLevels(GJSearchObject* searchObject, std::function<void(std::vector<Ref<GJGameLevel>>, bool)> callback, bool cacheLevels = true);
    BI_DLL cocos2d::CCArray* getStoredOnlineLevels(const std::string& key);
    BI_DLL void resetCache();
    BI_DLL bool showCFError(const std::string& data);
}