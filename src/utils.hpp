#pragma once

#include <Geode/utils/general.hpp>
using namespace geode::prelude;

#include "objects/BISearchObject.h"

#ifdef GEODE_IS_WINDOWS
    #ifdef BI_EXPORTING
        #define BI_DLL __declspec(dllexport)
    #else
        #define BI_DLL __declspec(dllimport)
    #endif
#else
    #ifdef BI_EXPORTING
        #define BI_DLL __attribute__((visibility("default")))
    #else
        #define BI_DLL
    #endif
#endif

#include "utils/LevelMetadata.h"
#include "utils/LevelProgressDialog.h"
#include "utils/LevelUtils.h"
#include "utils/ServerUtils.h"
#include "utils/TimeUtils.h"

namespace BetterInfo {
    BI_DLL CCSprite* createWithBISpriteFrameName(const char* name);
    BI_DLL CCSprite* createBISprite(const char* name);
    BI_DLL CCSprite* createPlaceholder();

    BI_DLL CCMenuItemSpriteExtra* createTextButton(cocos2d::CCLayer* parent, const char* text, cocos2d::SEL_MenuHandler handler, int width, float height, float scale);
    BI_DLL CCMenuItemSpriteExtra* createSearchButton(cocos2d::CCLayer* parent, const char* text, const char* icon, cocos2d::SEL_MenuHandler handler, float scale = .4f, float iconScale = 1.0f);

    BI_DLL int randomNumber(int start, int end);
    BI_DLL void strToLower(std::string& str);

    BI_DLL const char* rankIcon(int position);

    BI_DLL int levelsPerPage(GJSearchObject* searchObj);

    BI_DLL bool isLocal(GJSearchObject* searchObj);
    BI_DLL bool isFalseTotal(GJSearchObject* searchObj);
    BI_DLL bool isStarUseless(GJSearchObject* searchObj);
    BI_DLL bool isAdvancedEnabled(GJSearchObject* searchObj);

    BI_DLL bool isSavedFiltered();

    BI_DLL std::string decodeBase64Gzip(const std::string& input);
    BI_DLL std::string fileSize(size_t bytes);
    BI_DLL std::string fixColorCrashes(std::string input);
    BI_DLL std::string fixNullByteCrash(std::string input);
    
    BI_DLL void copyToClipboard(const char* text);
    BI_DLL void copyToClipboard(const char* text, CCLayer* parent);

    BI_DLL std::string getSongUrl(int audioID);
    BI_DLL bool isNewGrounds(int audioID);

    BI_DLL cocos2d::CCDictionary* responseToDict(const std::string& response);

    BI_DLL bool validateRangeItem(const BISearchObject::RangeItem& rangeItem, int value);
    BI_DLL bool levelMatchesObject(GJGameLevel* level, const BISearchObject& searchObj);
    BI_DLL bool levelProgressMatchesObject(GJGameLevel* level, const BISearchObject& searchObj);
    BI_DLL std::vector<GJGameLevel*> completedLevelsInStarRange(int min, int max, bool platformer, CCDictionary* dict = GameLevelManager::sharedState()->m_onlineLevels);

    BI_DLL void reloadUsernames(LevelBrowserLayer* levelBrowserLayer);

    BI_DLL uint64_t timeInMs();
    BI_DLL float timeForLevelString(const std::string& levelString);

    BI_DLL bool controllerConnected();

    BI_DLL std::string getWineVersion();
    BI_DLL std::string getOSVersion();
    BI_DLL void loadImportantNotices(Ref<CCLayer> layer);
    BI_DLL FLAlertLayer* createUpdateDialog();
    BI_DLL bool isHoveringNode(CCNode* target);

    BI_DLL bool isSprite(CCSprite* sprite, const char* name);
    BI_DLL CCMenuItemSpriteExtra* replaceWithButton(CCNode* node, CCNode* self, cocos2d::SEL_MenuHandler handler);
    BI_DLL std::string getNodeName(CCObject* node, bool spr);

    BI_DLL UnlockType iconTypeToUnlockType(IconType type);
    BI_DLL bool isLevelSearchObject(GJSearchObject* object);

    BI_DLL AxisLayoutOptions* copyLayoutOptions(CCNode* a);
    BI_DLL AxisLayoutOptions* copyLayoutOptions(AxisLayoutOptions* a);

    BI_DLL int stoi(std::string_view str);
    BI_DLL float stof(std::string_view str);
    BI_DLL long long strtol(std::string_view str);

    BI_DLL void showUnimportantNotification(const std::string& content, NotificationIcon icon, float time = 5.f);
    BI_DLL void cancelUnimportantNotifications();
}