#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace LevelProgressDialog {
    std::string printableProgress(std::string personalBests, int percentage);
    void show(GJGameLevel* level);
}