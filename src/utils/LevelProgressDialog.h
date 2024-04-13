#pragma once

#include <Geode/Geode.hpp>
#include "../utils.hpp"

using namespace geode::prelude;

namespace LevelProgressDialog {
    BI_DLL std::string printableProgress(std::string personalBests, int percentage);
    BI_DLL void show(GJGameLevel* level);
}