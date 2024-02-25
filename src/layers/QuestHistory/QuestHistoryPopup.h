#pragma once
#include "../_bases/CvoltonAlertLayerStub.h"

class QuestHistoryPopup : public CvoltonAlertLayerStub {
public:
    static QuestHistoryPopup* create();
    bool init();
};