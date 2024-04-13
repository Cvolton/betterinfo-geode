#pragma once
#include "../_bases/CvoltonAlertLayerStub.h"
#include "../../delegates/IDRangeDelegate.h"

class BI_DLL GameVersionPopup : public CvoltonAlertLayerStub {
    IDRangeDelegate* m_delegate;
    int m_min = 0;
    int m_max = 0;
    int m_additional = 0;
public:
    static GameVersionPopup* create(IDRangeDelegate* delegate, int min, int max, int additional);
    void onClose(cocos2d::CCObject* sender);
    bool init(IDRangeDelegate* delegate, int min, int max, int additional);
};