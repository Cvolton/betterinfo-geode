#pragma once

#include <string>
#include "../utils.hpp"

namespace LevelMetadata {
    BI_DLL std::string getGameVersionName(int version);
    BI_DLL std::string stringDate(std::string date);
    BI_DLL const char* getDifficultyIcon(int stars);
    BI_DLL const char* getDemonDifficultyIcon(int demonDifficulty);
    BI_DLL std::string passwordString(int password);
    BI_DLL std::string zeroIfNA(int value);
    BI_DLL std::string addPlus(std::string date);
    BI_DLL const char* boolString(bool value);
}