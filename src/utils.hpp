#pragma once

#include <Geode/utils/general.hpp>
using namespace geode::prelude;

#include "objects/BISearchObject.h"

namespace BetterInfo {
    CCSprite* createWithBISpriteFrameName(const char* name);
    CCSprite* createBISprite(const char* name);
    CCSprite* createPlaceholder();

    CCMenuItemSpriteExtra* createTextButton(cocos2d::CCLayer* parent, const char* text, cocos2d::SEL_MenuHandler handler, int width, float height, float scale);

    int randomNumber(int start, int end);
    void strToLower(std::string& str);

    const char* rankIcon(int position);

    int levelsPerPage(GJSearchObject* searchObj);

    bool isLocal(GJSearchObject* searchObj);
    bool isFalseTotal(GJSearchObject* searchObj);
    bool isStarUseless(GJSearchObject* searchObj);
    bool isAdvancedEnabled(GJSearchObject* searchObj);

    bool isSavedFiltered();

    std::string decodeBase64Gzip(const std::string& input);
    std::string fileSize(size_t bytes);
    std::string fixColorCrashes(std::string input);
    std::string fixNullByteCrash(std::string input);
    std::string timeToString(time_t input);
    std::string isoTimeToString(const std::string& input);
    
    void copyToClipboard(const char* text);
    void copyToClipboard(const char* text, CCLayer* parent);

    std::string getSongUrl(int audioID);
    bool isNewGrounds(int audioID);

    cocos2d::CCDictionary* responseToDict(const std::string& response);

    GJGameLevel* getLevelFromSaved(int levelID);
    std::deque<GJGameLevel*> completedDeque();

    int levelDifficultyAsInt(GJGameLevel* level);
    int levelDemonDifficultyAsInt(GJGameLevel* level);
    bool levelHasCollectedCoins(GJGameLevel* level);

    bool validateRangeItem(const BISearchObject::RangeItem& rangeItem, int value);
    bool levelMatchesObject(GJGameLevel* level, const BISearchObject& searchObj);
    bool levelProgressMatchesObject(GJGameLevel* level, const BISearchObject& searchObj);

    void reloadUsernames(LevelBrowserLayer* levelBrowserLayer);

    float timeForLevelString(const std::string& levelString);

    bool controllerConnected();

    void loadImportantNotices(CCLayer* layer);
    bool isHoveringNode(CCNode* target);
}