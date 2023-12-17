#pragma once

#include "../_bases/BIViewLayer.h"
#include "RewardCell.h"
#include "../../delegates/PageNumberDelegate.h"

class RewardViewLayer : public BIViewLayer {
protected:
    virtual bool init(cocos2d::CCDictionary* chests, const char* title);
public:
    static RewardViewLayer* create(cocos2d::CCDictionary* chests, const char* title);
    static cocos2d::CCScene* scene(cocos2d::CCDictionary* chests, const char* title);
    virtual void loadPage(unsigned int page);
    static bool compareRewards(const void* l1, const void* l2);
};