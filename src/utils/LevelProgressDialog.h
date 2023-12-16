#pragma once

#include <Geode/Bindings.hpp>

using namespace geode::prelude;

namespace LevelProgressDialog {
    std::string printableProgress(std::string personalBests, int percentage);
    void show(GJGameLevel* level);
}