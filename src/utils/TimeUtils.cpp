#include "TimeUtils.h"

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/chrono.h>
#include <sstream>
#include <time.h>
#include <chrono>

#ifndef GEODE_IS_ANDROID
#include <sys/timeb.h>
#endif

// using namespace std chrono for the whole cpp is fine
// since this is a dedicated utility file to time operations
using namespace std::chrono;

std::string TimeUtils::timeToString(time_t input) {
    struct tm timeInfo;
    #ifdef GEODE_IS_WINDOWS
        localtime_s(&timeInfo, &input);
    #else
        localtime_r(&input, &timeInfo);
    #endif

    return fmt::format("{:%F %H:%M}", timeInfo);
}

std::string TimeUtils::isoTimeToString(const std::string& input) {
    if(input.empty()) return "NA";
    auto pos = input.find('T');
    return pos != std::string::npos ? input.substr(0, pos) : input;
}

std::string TimeUtils::workingTime(int value){
    if(value < 0) return fmt::format("NA ({})", value);
    if(value == 0) return "NA";

    int hours = value / 3600;
    int minutes = (value % 3600) / 60;
    int seconds = value % 60;

    std::string res;
    if(hours > 0) res += fmt::format("{}h ", hours);
    if(hours > 0 || minutes > 0) res += fmt::format("{}m ", minutes);
    res += fmt::format("{}s", seconds);

    return res;
}

std::string TimeUtils::platformerTime(int value, bool showMilliseconds){
    if(value < 0) return fmt::format("NA ({})", value);
    if(value == 0) return "NA";

    int milliseconds = value % 1000;
    int seconds = (value / 1000) % 60;
    int minutes = (value / 60000) % 60;
    int hours = (value / 3600000);

    std::string millisecondsStr = showMilliseconds ? fmt::format(".{:03}", milliseconds) : "";

    if(hours > 0) return fmt::format("{:02}:{:02}:{:02}{}", hours, minutes, seconds, millisecondsStr);
    if(minutes > 0) return fmt::format("{:02}:{:02}{}", minutes, seconds, millisecondsStr);
    return fmt::format("{:02}{}", seconds, millisecondsStr);
}

std::string TimeUtils::minutesSeconds(int value){
    if(value < 0) return fmt::format("NA ({})", value);
    if(value == 0) return "NA";

    int minutes = value / 60;
    int seconds = value % 60;

    return fmt::format("{:02}:{:02}", minutes, seconds);
}

std::string TimeUtils::timestampToHumanReadable(time_t timestamp) {
    auto diff = difftime(time(nullptr), timestamp);

    int years = diff / 31536000;
    int months = diff / 2592000;
    int days = diff / 86400;
    int hours = diff / 3600;
    int minutes = diff / 60;

    if(years > 0) return fmt::format("{} year{}", years, years > 1 ? "s" : "");
    if(months > 0) return fmt::format("{} month{}", months, months > 1 ? "s" : "");
    if(days > 0) return fmt::format("{} day{}", days, days > 1 ? "s" : "");
    if(hours > 0) return fmt::format("{} hour{}", hours, hours > 1 ? "s" : "");
    if(minutes > 0) return fmt::format("{} minute{}", minutes, minutes > 1 ? "s" : "");
    return "Less than 1 minute";
}

double TimeUtils::getRobTopTime() {
    // For an unknown reason, RobTop only uses the bottom 20 bits of the time
    // This implementation gets the current timestamp that is compatible
    // with things like GLM->m_challengeTime

    #ifdef GEODE_IS_ANDROID
        // Remember what I just said?
        // This is a lie on Android! Rob uses the full timestamp there!
        return TimeUtils::getFullDoubleTime();
    #else
        struct timeb timebuffer;
        ftime(&timebuffer);
        return (timebuffer.time & 0xfffff) + timebuffer.millitm / 1000.0;
    #endif
}

double TimeUtils::getFullDoubleTime() {
    // Same output as getRobTopTime, but with the full timestamp

    #ifdef GEODE_IS_ANDROID
        struct timespec ts;
        clock_gettime(0, &ts);
        return ts.tv_sec + ts.tv_nsec / 1000000000.0;
    #else
        struct timeb timebuffer;
        ftime(&timebuffer);
        return timebuffer.time + timebuffer.millitm / 1000.0;
    #endif
}