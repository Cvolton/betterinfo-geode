#include "QuestHistoryObject.h"

QuestHistoryObject* QuestHistoryObject::create(int diamondCount, int questCount) {
    auto ret = new QuestHistoryObject();
    if (ret) {
        ret->init(diamondCount, questCount);
    }
    return ret;
}

bool QuestHistoryObject::init(int diamondCount, int questCount) {
    m_diamondCount = diamondCount;
    m_questCount = questCount;
    return true;
}