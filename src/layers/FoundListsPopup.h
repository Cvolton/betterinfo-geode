#pragma once
#include "_bases/CvoltonAlertLayerStub.h"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class BI_DLL FoundListsPopup : public CvoltonAlertLayerStub {
    std::unordered_set<int> m_levels;
public:
    static FoundListsPopup* create();
    void onClose(cocos2d::CCObject* sender);
    void onOK(cocos2d::CCObject* sender);
    bool init();
    static cocos2d::CCLabelBMFont* createTextLabel(const std::string text, const cocos2d::CCPoint& position, const float scale, cocos2d::CCNode* menu, const char* font = "bigFont.fnt");
};