#pragma once

#include <Geode/Geode.hpp>
#include "RewardCategoryObject.h"

using namespace geode::prelude;

class BI_DLL RewardCategoryCell : public GenericListCell {
        Ref<RewardCategoryObject> m_category;
    public:
        RewardCategoryCell(const char* name, cocos2d::CCSize size);
        void loadFromData(CCObject* object);
        void onView(CCObject* sender);
        static RewardCategoryCell* create(const char* key, cocos2d::CCSize size);
};