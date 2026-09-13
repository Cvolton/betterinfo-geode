#include "CulledScoreListView.h"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

CulledScoreListView* CulledScoreListView::create(cocos2d::CCArray* data, float width, float height, int accountID) {
    auto ret = new CulledScoreListView();
    if (accountID > 0) {
        ret->m_accountID = accountID;
    } else {
        ret->m_accountID = GJAccountManager::sharedState()->m_accountID;
    }
    ret->init(data, width, height, 55.f);
    ret->autorelease();
    return ret;
}

void CulledScoreListView::setupList(float separation) {
    CvoltonListView::setupList(separation);

    size_t idx = 1;
    bool found = false;
    for(auto entry : m_entries->asExt<GJUserScore*>()) {
        if(entry->m_accountID == m_accountID) {
            found = true;
            break;
        }
        idx++;
    }

    if(idx > m_entries->count() - 1) {
        idx = m_entries->count() - 1;
    };

    if(found) {
        float targetY = (idx * m_itemSeparation - m_height * 0.5f)
              - (m_itemSeparation * 0.5f);

        auto pos = m_tableView->m_contentLayer->getPosition();
        m_tableView->setContentOffset({pos.x, targetY + pos.y}, false);
    }
}