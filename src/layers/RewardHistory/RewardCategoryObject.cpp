#include "RewardCategoryObject.h"
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