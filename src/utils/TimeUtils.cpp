#include "TimeUtils.h"

#include <fmt/format.h>

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