#pragma once
#include "CvoltonAlertLayerStub.h"

class LevelIDLayer : public CvoltonAlertLayerStub {
    CCTextInputNode* m_textNode;
public:
    static LevelIDLayer* create();
    void onClose(cocos2d::CCObject* sender);
    void onOK(cocos2d::CCObject* sender);
    bool init();
    int getNumber();
    static cocos2d::CCLabelBMFont* createTextLabel(const std::string text, const cocos2d::CCPoint& position, const float scale, cocos2d::CCNode* menu, const char* font = "bigFont.fnt");
};