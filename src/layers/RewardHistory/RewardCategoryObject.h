#pragma once

#include <Geode/Geode.hpp>
#include "../../hooks/GJRewardItem.h"
#include "Geode/cocos/cocoa/CCDictionary.h"

using namespace geode::prelude;

class RewardCategoryObject : public CCObject {
        cocos2d::CCDictionary* m_filteredChests = nullptr;
    public:
        std::string m_title;
        std::string m_icon;
        cocos2d::CCDictionary* m_chests = nullptr;
        std::function<bool(BIGJRewardItem*)> m_filter;

        static RewardCategoryObject* create(std::string_view title, std::string_view icon, cocos2d::CCDictionary* chests, std::function<bool(BIGJRewardItem*)> filter);
        bool init(std::string_view title, std::string_view icon, cocos2d::CCDictionary* chests, std::function<bool(BIGJRewardItem*)> filter);
        cocos2d::CCDictionary* filteredChests();
        std::map<SpecialRewardItem, int> countSpecialItems();
        ~RewardCategoryObject();
};