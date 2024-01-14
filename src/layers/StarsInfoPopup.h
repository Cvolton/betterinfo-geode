#pragma once
#include "_bases/CvoltonAlertLayerStub.h"

class StarsInfoPopup : public CvoltonAlertLayerStub {
public:
    static StarsInfoPopup* create();
    bool init();
    int completedLevelsForDifficulty(int difficulty, bool platformer);
};