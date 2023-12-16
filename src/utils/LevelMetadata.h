#pragma once

#include <string>

namespace LevelMetadata {
    std::string getGameVersionName(int version);
    std::string stringDate(std::string date);
    const char* getDifficultyIcon(int stars);
    const char* getDemonDifficultyIcon(int demonDifficulty);
    std::string passwordString(int password);
    std::string zeroIfNA(int value);
    std::string addPlus(std::string date);
    const char* boolString(bool value);
}