#include "CulledScoreCell.h"

void CulledScoreCell::loadFromData(CCObject* object) {
    m_score = static_cast<GJUserScore*>(object);
}

void CulledScoreCell::draw() {
    if(!m_scoreCell && m_score) {
        auto cell = new GJScoreCell("g", m_width, m_height);
        cell->autorelease();
        cell->loadFromScore(m_score);
        m_mainLayer->addChild(cell);
        m_scoreCell = cell;
    }
    reinterpret_cast<StatsCell*>(this)->StatsCell::draw();
}

void CulledScoreCell::setVisible(bool visible) {
    GenericListCell::setVisible(visible);

    // this breaks better touch prio
    /*if(m_scoreCell && !visible) {
        m_scoreCell->removeFromParent();
        m_scoreCell = nullptr;
    }*/
}

CulledScoreCell::CulledScoreCell(const char* name, CCSize size) :
    GenericListCell(name, size) {}

CulledScoreCell* CulledScoreCell::create(const char* key, CCSize size) {
    auto ret = new CulledScoreCell(key, size);
    if (ret && ret->init()) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}