#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class DailyCell : public TableViewCell {
        GJGameLevel* m_level;
        
        void draw() override;
        void onView(cocos2d::CCObject* sender);
        void onLeaderboards(cocos2d::CCObject* sender);
        void onInfo(cocos2d::CCObject* sender);
        int getTotalDiamonds();
        int getAwardedDiamonds();
    
    public:
        DailyCell(const char* name, cocos2d::CCSize size);
        void loadFromData(CCObject* object);
        static DailyCell* create(const char* key, cocos2d::CCSize size);
};