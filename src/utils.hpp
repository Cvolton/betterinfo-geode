#pragma once

#include <Geode/utils/general.hpp>
using namespace geode::prelude;

namespace BetterInfo {
    CCSprite* createWithBISpriteFrameName(const char* name);
    CCSprite* createBISprite(const char* name);
    CCSprite* createPlaceholder();
    
    int randomNumber(int start, int end);

    const char* rankIcon(int position);

    int levelsPerPage(GJSearchObject* searchObj);

    bool isLocal(GJSearchObject* searchObj);
    bool isFalseTotal(GJSearchObject* searchObj);
    bool isStarUseless(GJSearchObject* searchObj);

    bool isSavedFiltered();

    std::string decodeBase64Gzip(const std::string& input);
    std::string fileSize(size_t bytes);
    std::string fixColorCrashes(std::string input);
    std::string fixNullByteCrash(std::string input);
    std::string timeToString(time_t input);
    
    void copyToClipboard(const char* text);
    void copyToClipboard(const char* text, CCLayer* parent);

    std::string getSongUrl(int audioID);
    bool isNewGrounds(int audioID);

    cocos2d::CCDictionary* responseToDict(const std::string& response);
}