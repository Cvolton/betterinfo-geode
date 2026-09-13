#include "CulledScoreCell.h"

void CulledScoreCell::loadFromData(CCObject* object) {
    m_score = static_cast<GJUserScore*>(object);

    if(m_score && m_score->m_userID == m_idToHighlight) {
        m_score->setUserFlag("jump-here"_spr);
    }
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

void CulledScoreCell::updateBGColor(int idx) {
    GenericListCell::updateBGColor(idx);

    if(m_score && m_score->getUserFlag("jump-here"_spr)) {
        m_backgroundLayer->setColor({230, 150, 10});
    }
}