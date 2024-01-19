#pragma once

#include <Geode/utils/general.hpp>
using namespace geode::prelude;

#include "objects/BISearchObject.h"

#include "utils/LevelMetadata.h"
#include "utils/LevelProgressDialog.h"
#include "utils/LevelUtils.h"
#include "utils/TimeUtils.h"

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
    
    void copyToClipboard(const char* text);
    void copyToClipboard(const char* text, CCLayer* parent);

    std::string getSongUrl(int audioID);
    bool isNewGrounds(int audioID);

    cocos2d::CCDictionary* responseToDict(const std::string& response);

    bool validateRangeItem(const BISearchObject::RangeItem& rangeItem, int value);
    bool levelMatchesObject(GJGameLevel* level, const BISearchObject& searchObj);
    bool levelProgressMatchesObject(GJGameLevel* level, const BISearchObject& searchObj);
    std::vector<GJGameLevel*> completedLevelsInStarRange(int min, int max, bool platformer, CCDictionary* dict = GameLevelManager::sharedState()->m_onlineLevels);

    void reloadUsernames(LevelBrowserLayer* levelBrowserLayer);

    float timeForLevelString(const std::string& levelString);

    bool controllerConnected();

    void loadImportantNotices(CCLayer* layer);
    bool isHoveringNode(CCNode* target);

    bool isSprite(CCSprite* sprite, const char* name);
    CCMenuItemSpriteExtra* replaceWithButton(CCNode* node, CCNode* self, cocos2d::SEL_MenuHandler handler);

    UnlockType iconTypeToUnlockType(IconType type);

    AxisLayoutOptions* copyLayoutOptions(CCNode* a);
    AxisLayoutOptions* copyLayoutOptions(AxisLayoutOptions* a);

    int stoi(std::string_view str);
}