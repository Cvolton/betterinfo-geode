#pragma once

#include "../_bases/BIViewLayer.h"

class BI_DLL RewardViewLayer : public BIViewLayer {
protected:
    virtual bool init(cocos2d::CCDictionary* chests, const char* title);
public:
    static RewardViewLayer* create(cocos2d::CCDictionary* chests, const char* title);
    static cocos2d::CCScene* scene(cocos2d::CCDictionary* chests, const char* title);
    virtual void loadPage();
    static bool compareRewards(const void* l1, const void* l2);
};