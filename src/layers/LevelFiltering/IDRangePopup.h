#pragma once
#include "../_bases/CvoltonAlertLayerStub.h"
#include "../../delegates/IDRangeDelegate.h"

class BI_DLL IDRangePopup : public CvoltonAlertLayerStub {
    IDRangeDelegate* m_delegate;
    CCTextInputNode* m_minNode = nullptr;
    CCTextInputNode* m_maxNode = nullptr;
    int m_additional = 0;
public:
    static IDRangePopup* create(IDRangeDelegate* delegate, int min, int max, const char* text, int additional = 0, bool single = false);
    void onClose(cocos2d::CCObject* sender);
    bool init(IDRangeDelegate* delegate, int min, int max, const char* text, int additional = 0, bool single = false);
    int minID();
    int maxID();
};