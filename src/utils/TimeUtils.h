#pragma once

#include <time.h>
#include <string>
#include "../utils.hpp"

namespace TimeUtils {
    BI_DLL std::string timeToString(time_t input);
    BI_DLL std::string isoTimeToString(const std::string& input);
    BI_DLL std::string workingTime(int value);
    BI_DLL std::string platformerTime(int value, bool showMilliseconds = true);
    BI_DLL std::string minutesSeconds(int value);
    BI_DLL std::string timestampToHumanReadable(time_t timestamp);

    BI_DLL double getRobTopTime();
    BI_DLL double getFullDoubleTime();
}