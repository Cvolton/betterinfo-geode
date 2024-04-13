#pragma once
#include "../_bases/CvoltonAlertLayerStub.h"

class BI_DLL QuestHistoryPopup : public CvoltonAlertLayerStub {
public:
    static QuestHistoryPopup* create();
    bool init();
    void show();
};