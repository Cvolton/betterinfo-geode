#pragma once

#include <Geode/Geode.hpp>
#include "../utils.hpp"

using namespace geode::prelude;

class BI_DLL CulledScoreCell : public GenericListCell {
        Ref<GJUserScore> m_score = nullptr;
        GJScoreCell* m_scoreCell = nullptr;
        
        void draw() override;
        void setVisible(bool visible) override;
    public:
        CulledScoreCell(const char* name, cocos2d::CCSize size);
        void loadFromData(CCObject* object);
        static CulledScoreCell* create(const char* key, cocos2d::CCSize size);
};