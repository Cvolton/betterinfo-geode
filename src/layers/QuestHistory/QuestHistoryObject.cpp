#include "QuestHistoryObject.h"

QuestHistoryObject* QuestHistoryObject::create(int diamondCount, int questCount) {
    auto ret = new QuestHistoryObject();
    ret->init(diamondCount, questCount);
    ret->autorelease();
    return ret;
}

bool QuestHistoryObject::init(int diamondCount, int questCount) {
    m_diamondCount = diamondCount;
    m_questCount = questCount;
    return true;
}