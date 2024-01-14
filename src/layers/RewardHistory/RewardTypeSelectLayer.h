#pragma once
#include "../_bases/CvoltonAlertLayerStub.h"


class RewardTypeSelectLayer : public CvoltonAlertLayerStub {
public:
    static RewardTypeSelectLayer* create();
    void onDaily(cocos2d::CCObject* sender);
    void onTreasure(cocos2d::CCObject* sender);
    void onWeekly(cocos2d::CCObject* sender);
    void onOther(cocos2d::CCObject* sender);
    bool init();
};