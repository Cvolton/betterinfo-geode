#pragma once

#include <Geode/Bindings.hpp>

using namespace geode::prelude;

namespace LevelUtils {
    GJGameLevel* getLevelFromSaved(int levelID);
    std::deque<GJGameLevel*> completedDeque();

    int levelDifficultyAsInt(GJGameLevel* level);
    int levelDemonDifficultyAsInt(GJGameLevel* level);
    bool levelHasCollectedCoins(GJGameLevel* level);
}