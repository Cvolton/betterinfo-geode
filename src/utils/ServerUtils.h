#pragma once

#include <Geode/Geode.hpp>

namespace ServerUtils {
    std::string getBaseURL();
    void getOnlineLevels(GJSearchObject* searchObject, std::function<void(std::vector<GJGameLevel*>, bool)> callback);
    bool showCFError(const std::string& data);
}