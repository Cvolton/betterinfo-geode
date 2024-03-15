#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace ServerUtils {
    std::string getBaseURL();
    void getOnlineLevels(GJSearchObject* searchObject, std::function<void(std::vector<Ref<GJGameLevel>>, bool)> callback);
    bool showCFError(const std::string& data);
}