#include "LevelMetadata.h"

std::string LevelMetadata::getGameVersionName(int version){
    if(version < 1 || version > 99) return std::string("NA");

    switch(version){
        case 10:
            return std::string("1.7");
        case 11:
            return std::string("Early 1.8");
    }

    std::stringstream contentStream;

    if(version > 17){
        double newVersion = (double)version / 10.0;
        contentStream << std::fixed << std::setprecision(1) << newVersion;
    }else{
        contentStream << "1." << (version-1);
    }
    return contentStream.str();
}

std::string LevelMetadata::stringDate(std::string date){
    if(date == "") return "NA";
    std::ostringstream stream;
    stream << date << " ago";
    return stream.str();
}

const char* LevelMetadata::getDifficultyIcon(int stars){
    switch(stars){
        case 1: 
            return "difficulty_auto_btn_001.png";
        case 2:
            return "difficulty_01_btn_001.png";
        case 3:
            return "difficulty_02_btn_001.png";
        case 4:
        case 5:
            return "difficulty_03_btn_001.png";
        case 6:
        case 7:
            return "difficulty_04_btn_001.png";
        case 8:
        case 9:
            return "difficulty_05_btn_001.png";
        case 10:
            return "difficulty_06_btn_001.png";
        default:
            return "difficulty_00_btn_001.png";
    }
}

const char* LevelMetadata::getDemonDifficultyIcon(int demonDifficulty){
    switch(demonDifficulty){
        case 3: 
            return "difficulty_07_btn_001.png";
        case 4:
            return "difficulty_08_btn_001.png";
        case 5:
            return "difficulty_09_btn_001.png";
        case 6:
            return "difficulty_10_btn_001.png";
        default:
            return "difficulty_06_btn_001.png";
    }
}

std::string LevelMetadata::passwordString(int password){
    if(password == 0) return "NA";
    if(password == 1) return "Free Copy";
    if(password >= 10000 && password <= 19999) return std::to_string(password - 10000);
    if(password >= 1000000 && password <= 1999999) return std::to_string(password - 1000000);
    return "Invalid (" + std::to_string(password) + ")";
}

std::string LevelMetadata::zeroIfNA(int value){
    if(value == 0) return "NA";
    return std::to_string(value);
}

std::string LevelMetadata::addPlus(std::string date) {
    auto spaceIt = date.find_first_of(' ');
    if(spaceIt != std::string::npos) date.insert(date.cbegin() + spaceIt, '+');
    return date;
}

const char* LevelMetadata::boolString(bool value) {
    return value ? "True" : "False";
}