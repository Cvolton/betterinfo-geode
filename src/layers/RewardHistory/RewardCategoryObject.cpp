#include "RewardCategoryObject.h"
#include "Geode/Enums.hpp"
#include "Geode/cocos/cocoa/CCDictionary.h"

RewardCategoryObject* RewardCategoryObject::create(std::string_view title, std::string_view icon, cocos2d::CCDictionary* chests, std::function<bool(BIGJRewardItem*)> filter) {
    auto ret = new RewardCategoryObject();
    if (ret && ret->init(title, icon, chests, filter)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool RewardCategoryObject::init(std::string_view title, std::string_view icon, cocos2d::CCDictionary* chests, std::function<bool(BIGJRewardItem*)> filter) {
    m_title = title;
    m_icon = icon;
    m_filter = filter;
    m_chests = chests;

    m_chests->retain();

    return true;
}

cocos2d::CCDictionary* RewardCategoryObject::filteredChests() {
    if (!m_filteredChests) {
        m_filteredChests = cocos2d::CCDictionary::create();
        m_filteredChests->retain();

        for (auto&& [key, chest] : CCDictionaryExt<gd::string, BIGJRewardItem*>(m_chests)) {
            chest->setKey(key);
            if (m_filter(chest)) {
                m_filteredChests->setObject(chest, key);
            }
        }
    }

    return m_filteredChests;
}

RewardCategoryObject::~RewardCategoryObject() {
    m_chests->release();
    if (m_filteredChests) {
        m_filteredChests->release();
    }
}

std::map<SpecialRewardItem, int> RewardCategoryObject::countSpecialItems() {
    std::map<SpecialRewardItem, int> counts;

    for (auto&& [key, chest] : CCDictionaryExt<gd::string, BIGJRewardItem*>(filteredChests())) {
        for(auto&& reward : CCArrayExt<GJRewardObject*>(chest->m_rewardObjects)) {
            auto item = reward->m_specialRewardItem;
            if(((int) item >= 0x1 && (int) item <= 0x5) || ((int) item >= 0xA && (int) item <= 0xE)) {
                item = (SpecialRewardItem) -1;
            }

            counts[item] += reward->m_total;
        }
    }

    return counts;
}