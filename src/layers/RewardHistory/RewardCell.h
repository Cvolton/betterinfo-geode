#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class RewardCell : public GenericListCell {
    public:
        RewardCell(const char* name, cocos2d::CCSize size);
        void loadFromData(CCObject* object);
        static RewardCell* create(const char* key, cocos2d::CCSize size);
};