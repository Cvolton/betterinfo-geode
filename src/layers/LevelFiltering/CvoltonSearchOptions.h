#pragma once
#include "../_bases/CvoltonOptionsLayer.h"
#include "../../delegates/IDRangeDelegate.h"

class BI_DLL CvoltonSearchOptions : public CvoltonOptionsLayer, public IDRangeDelegate {
public:
    static CvoltonSearchOptions* create();
    void onClose(cocos2d::CCObject* sender);
    void onPrev(cocos2d::CCObject* sender);
    void onInfo(cocos2d::CCObject* sender);
    bool init();
    void drawToggles();
    void onPercentageRange(cocos2d::CCObject* sender);
    std::string getCompletedString();

    void onIDRangeFinished(int min, int max, int additional);
};