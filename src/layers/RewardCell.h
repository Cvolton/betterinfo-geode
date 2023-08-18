#pragma once

#include <Geode/Bindings.hpp>

using namespace geode::prelude;

class RewardCell : public TableViewCell {
        GJGameLevel* level;
        void draw() override;
    
    public:
        RewardCell(const char* name, cocos2d::CCSize size);
        void loadFromReward(GJRewardItem* reward);
        static RewardCell* create(const char* key, cocos2d::CCSize size);
};