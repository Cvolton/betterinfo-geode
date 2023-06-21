#pragma once
#include "CvoltonAlertLayerStub.h"


class RewardTypeSelectLayer : public CvoltonAlertLayerStub {
public:
    static RewardTypeSelectLayer* create();
    void onClose(cocos2d::CCObject* sender);
    void onDaily(cocos2d::CCObject* sender);
    void onTreasure(cocos2d::CCObject* sender);
    void onWeekly(cocos2d::CCObject* sender);
    void onOther(cocos2d::CCObject* sender);
    bool init();
};