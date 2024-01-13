#pragma once
#include "_bases/CvoltonAlertLayerStub.h"

class StarsInfoPopup : public CvoltonAlertLayerStub {
public:
    static StarsInfoPopup* create();
    bool init();
    void onClose(CCObject* sender);
    int completedLevelsForDifficulty(int difficulty, bool platformer);
};