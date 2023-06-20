#pragma once
#include <Geode/Bindings.hpp>

using namespace geode::prelude;

#include "CvoltonAlertLayerStub.h"

class ExtendedLevelInfo : public CvoltonAlertLayerStub {
    std::string m_primary;
    std::string m_secondary;
    GJGameLevel* m_level;
    TextArea* m_info;
    CCMenuItemSpriteExtra* m_prevBtn;
    CCMenuItemSpriteExtra* m_nextBtn;
    int m_page = 0;
public:
    static ExtendedLevelInfo* create(GJGameLevel* level);
    void onClose(cocos2d::CCObject* sender);
    void onCopyName(cocos2d::CCObject* sender);
    void onCopyAuthor(cocos2d::CCObject* sender);
    void onCopyDesc(cocos2d::CCObject* sender);
    void onNext(cocos2d::CCObject* sender);
    void onPrev(cocos2d::CCObject* sender);
    void loadPage(int page);
    static std::string getGameVersionName(int version);
    static std::string stringDate(std::string date);
    static const char* boolString(bool value);
    static const char* getDifficultyIcon(int stars);
    static std::string passwordString(int password);
    static std::string zeroIfNA(int value);
    static std::string workingTime(int value);
    static std::string printableProgress(std::string personalBests, int percentage);
    static void showProgressDialog(GJGameLevel* level);
    bool init(GJGameLevel* level);
    static cocos2d::CCLabelBMFont* createTextLabel(const std::string text, const cocos2d::CCPoint& position, const float scale, cocos2d::CCNode* menu, const char* font = "bigFont.fnt");
};