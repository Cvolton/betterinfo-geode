#include "TimeUtils.h"

#include <fmt/format.h>
#include <sstream>
#include <sys/timeb.h>
#include <time.h>

std::string TimeUtils::timeToString(time_t input) {
    char dateString[255];
    strftime(dateString, sizeof(dateString), "%F %H:%M", localtime(&input));
    return dateString;
}

std::string TimeUtils::isoTimeToString(const std::string& input) {
    if(input.empty()) return "NA";
    int y,M,d;
    sscanf(input.c_str(), "%d-%d-%dT", &y, &M, &d);
    return fmt::format("{}-{:02}-{:02}", y, M, d);
}

std::string TimeUtils::workingTime(int value){
    if(value <= 0) return "NA";

    int hours = value / 3600;
    int minutes = (value % 3600) / 60;
    int seconds = value % 60;

    std::ostringstream stream;
    if(hours > 0) stream << hours << "h ";
    if(minutes > 0) stream << minutes << "m ";
    stream << seconds << "s";

    return stream.str();
}

std::string TimeUtils::platformerTime(int value){
    int milliseconds = value % 1000;
    int seconds = (value / 1000) % 60;
    int minutes = (value / 60000) % 60;
    int hours = (value / 3600000);

    if(hours > 0) return fmt::format("{}:{}:{}.{}", hours, minutes, seconds, milliseconds);
    if(minutes > 0) return fmt::format("{}:{}.{}", minutes, seconds, milliseconds);
    return fmt::format("{}.{}", seconds, milliseconds);
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
    struct timeb timebuffer;
    ftime(&timebuffer);
    return (timebuffer.time & 0xfffff) + timebuffer.millitm / 1000.0;
}

double TimeUtils::getFullDoubleTime() {
    struct timeb timebuffer;
    ftime(&timebuffer);
    return timebuffer.time + timebuffer.millitm / 1000.0;
}