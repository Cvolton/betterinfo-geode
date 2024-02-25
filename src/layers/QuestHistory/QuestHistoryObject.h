#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class QuestHistoryObject : public CCObject {
    public:
        int m_diamondCount;
        int m_questCount;

        static QuestHistoryObject* create(int diamondCount, int questCount);
        bool init(int diamondCount, int questCount);
};