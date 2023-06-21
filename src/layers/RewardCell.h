#pragma once

#include <Geode/Bindings.hpp>

using namespace geode::prelude;

class RewardCell : public TableViewCell {
        GJGameLevel* level;
        RewardCell(const char* name, cocos2d::CCSize size);
        void draw() override;
    
    public:
        void loadFromReward(GJRewardItem* reward);
        static RewardCell* create(const char* key, cocos2d::CCSize size);
};