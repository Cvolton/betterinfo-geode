#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class QuestHistoryCell : public GenericListCell {
    public:
        QuestHistoryCell(const char* name, cocos2d::CCSize size);
        void loadFromData(CCObject* object);
        static QuestHistoryCell* create(const char* key, cocos2d::CCSize size);
        void updateBGColor(int index);
};