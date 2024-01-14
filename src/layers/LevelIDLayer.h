#pragma once
#include "_bases/CvoltonAlertLayerStub.h"

class LevelIDLayer : public CvoltonAlertLayerStub {
    CCTextInputNode* m_textNode;
public:
    static LevelIDLayer* create();
    void onLevelComments(cocos2d::CCObject* sender);
    void onAccountProfile(cocos2d::CCObject* sender);
    bool init();
    int getNumber();
    static cocos2d::CCLabelBMFont* createTextLabel(const std::string text, const cocos2d::CCPoint& position, const float scale, cocos2d::CCNode* menu, const char* font = "bigFont.fnt");
};