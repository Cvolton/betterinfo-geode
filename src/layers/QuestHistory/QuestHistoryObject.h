#pragma once

#include <Geode/Geode.hpp>
#include "../../utils.hpp"

using namespace geode::prelude;

class BI_DLL QuestHistoryObject : public CCObject {
    public:
        int m_diamondCount;
        int m_questCount;

        static QuestHistoryObject* create(int diamondCount, int questCount);
        bool init(int diamondCount, int questCount);
};