#pragma once
#include "_bases/CvoltonAlertLayerStub.h"

class BI_DLL StarsInfoPopup : public CvoltonAlertLayerStub {
public:
    static StarsInfoPopup* create();
    bool init();
    int completedLevelsForDifficulty(int difficulty, bool platformer);
};