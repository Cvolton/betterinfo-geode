#pragma once

#include <Geode/Geode.hpp>
#include "../../utils.hpp"

using namespace geode::prelude;

class BI_DLL RewardCell : public GenericListCell {
        GJRewardItem* m_rewardItem;
        
        std::string getDisplayName();
    public:
        RewardCell(const char* name, cocos2d::CCSize size);
        void loadFromData(CCObject* object);
        static RewardCell* create(const char* key, cocos2d::CCSize size);
        static CCSprite* itemToSprite(SpecialRewardItem item);
        void onIcon(CCObject* sender);
};