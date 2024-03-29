#pragma once

#include <time.h>
#include <string>

namespace TimeUtils {
    std::string timeToString(time_t input);
    std::string isoTimeToString(const std::string& input);
    std::string workingTime(int value);
    std::string platformerTime(int value, bool showMilliseconds = true);
    std::string minutesSeconds(int value);
    std::string timestampToHumanReadable(time_t timestamp);

    double getRobTopTime();
    double getFullDoubleTime();
}