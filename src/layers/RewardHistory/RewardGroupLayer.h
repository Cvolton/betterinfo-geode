#pragma once

#include "../_bases/BIViewLayer.h"
#include "../../hooks/GJRewardItem.h"

using RewardGroupCategoryVector = std::vector<std::tuple<const char*, const char*, std::function<bool(BIGJRewardItem*)>>>; // title, icon, filter

class RewardGroupLayer : public BIViewLayer {
protected:
    virtual bool init(const char* title, cocos2d::CCDictionary* chests, RewardGroupCategoryVector categories);
public:
    static RewardGroupLayer* create(const char* title, cocos2d::CCDictionary* chests, RewardGroupCategoryVector categories);
    static cocos2d::CCScene* scene(const char* title, cocos2d::CCDictionary* chests, RewardGroupCategoryVector categories);
    virtual void loadPage();
};