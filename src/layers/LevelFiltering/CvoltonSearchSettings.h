#pragma once
#include "../_bases/CvoltonOptionsLayer.h"

class BI_DLL CvoltonSearchSettings : public CvoltonOptionsLayer {
public:
    static CvoltonSearchSettings* create();
    void onClose(cocos2d::CCObject* sender);
    bool init();
    void drawToggles();
};