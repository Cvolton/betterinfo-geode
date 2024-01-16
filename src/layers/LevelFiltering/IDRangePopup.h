#pragma once
#include "../_bases/CvoltonAlertLayerStub.h"
#include "../../delegates/IDRangeDelegate.h"

class IDRangePopup : public CvoltonAlertLayerStub {
    IDRangeDelegate* m_delegate;
    CCTextInputNode* m_minNode = nullptr;
    CCTextInputNode* m_maxNode = nullptr;
    int m_additional = 0;
public:
    static IDRangePopup* create(IDRangeDelegate* delegate, int min, int max, const char* text, int additional = 0);
    void onClose(cocos2d::CCObject* sender);
    bool init(IDRangeDelegate* delegate, int min, int max, const char* text, int additional = 0);
    int minID();
    int maxID();
};