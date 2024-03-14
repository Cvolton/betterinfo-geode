#pragma once

#include <Geode/Geode.hpp>

namespace ServerUtils {
    const char* getBaseURL();
    void getOnlineLevels(GJSearchObject* searchObject, std::function<void(std::vector<GJGameLevel*>, bool)> callback);
}