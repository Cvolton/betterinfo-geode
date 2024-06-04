#pragma once

#include <Geode/Geode.hpp>
#include "../utils.hpp"

#include <deque>

using namespace geode::prelude;

namespace LevelUtils {
    BI_DLL GJGameLevel* getLevelFromSaved(int levelID);
    BI_DLL std::deque<GJGameLevel*> completedDeque();
    BI_DLL void resetLevelCaches();

    BI_DLL int levelDifficultyAsInt(GJGameLevel* level);
    BI_DLL int levelDemonDifficultyAsInt(GJGameLevel* level);
    BI_DLL bool levelHasCollectedCoins(GJGameLevel* level);
}