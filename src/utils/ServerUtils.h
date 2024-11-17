#pragma once

#include <Geode/Geode.hpp>
#include "../utils.hpp"

#include <Geode/utils/web.hpp>

using namespace geode::prelude;

namespace ServerUtils {
    BI_DLL bool isGDPS();
    BI_DLL web::WebRequest getBaseRequest(bool setUserAgent = true);
    BI_DLL std::string getBaseURL();
    BI_DLL std::string getBasePostString(bool includeAccount = true);
    BI_DLL std::string getSearchObjectKey(GJSearchObject* searchObject);
    BI_DLL void getLevelLists(GJSearchObject* searchObject, std::function<void(std::shared_ptr<std::vector<Ref<GJLevelList>>>, bool)> callback, bool cacheLevels = true);
    BI_DLL void getOnlineLevels(GJSearchObject* searchObject, std::function<void(std::shared_ptr<std::vector<Ref<GJGameLevel>>>, bool success, bool explicitError)> callback, bool cacheLevels = true);
    BI_DLL cocos2d::CCArray* getStoredOnlineLevels(const std::string& key);
    BI_DLL void resetCache();
    BI_DLL bool showCFError(const std::string& data);
    BI_DLL bool showRateLimitError(int seconds);
}