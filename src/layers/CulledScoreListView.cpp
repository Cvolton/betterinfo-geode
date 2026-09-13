#include "CulledScoreListView.h"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

CulledScoreListView* CulledScoreListView::create(cocos2d::CCArray* data, float width, float height, float separation) {
    auto ret = new CulledScoreListView();
    if (ret && ret->init(data, width, height, separation)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

void CulledScoreListView::setupList(float separation) {
    CvoltonListView::setupList(separation);

    size_t idx = 1;
    bool found = false;
    for(auto entry : m_entries->asExt<GJUserScore*>()) {
        if(entry->getUserFlag("jump-here"_spr)) {
            log::info("Found jump-here entry at index {}", idx);
            found = true;
            break;
        }
        idx++;
    }

    if(idx > m_entries->count() - 1) return;

    if(found) {
        float targetY = (idx * m_itemSeparation - m_height * 0.5f)
              - (m_itemSeparation * 0.5f);

        auto pos = m_tableView->m_contentLayer->getPosition();
        m_tableView->setContentOffset({pos.x, targetY + pos.y}, false);
    }
}