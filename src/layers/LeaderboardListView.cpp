#include "LeaderboardListView.h"

LeaderboardListView* LeaderboardListView::create(cocos2d::CCArray* scores, float width, float height) {
    auto ret = new LeaderboardListView();
    if (ret && ret->init(scores, 0x420, width, height)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

void LeaderboardListView::setupList() {
    this->m_itemSeparation = 60.0f;

    this->m_tableView->retain();
    this->m_tableView->reloadData();

    auto coverage = calculateNodeCoverage(m_tableView->m_contentLayer->getChildren());
    if (this->m_entries->count() > 4)
        m_tableView->m_contentLayer->setContentSize({-coverage.origin.x + coverage.size.width, -coverage.origin.y + coverage.size.height});

    this->m_tableView->moveToTop();

    if (this->m_entries->count() == 1)
        this->m_tableView->moveToTopWithOffset(this->m_itemSeparation);
}

void LeaderboardListView::loadCell(TableViewCell* cell, int index) {
    static_cast<GJScoreCell*>(cell)->loadFromScore(
        static_cast<GJUserScore*>(this->m_entries->objectAtIndex(index))
    );
    static_cast<GJScoreCell*>(cell)->updateBGColor(index);
}

TableViewCell* LeaderboardListView::getListCell(const char* key) {
    return new GJScoreCell(key, this->m_width, this->m_itemSeparation);
}