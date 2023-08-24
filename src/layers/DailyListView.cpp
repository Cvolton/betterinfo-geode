#include "DailyListView.h"
#include "DailyCell.h"

DailyListView* DailyListView::create(cocos2d::CCArray* levels, float width, float height) {
    auto ret = new DailyListView();
    if (ret && ret->init(levels, 0x420, width, height)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

void DailyListView::setupList() {
    this->m_itemSeparation = 55.0f;

    this->m_tableView->reloadData();

    auto coverage = calculateNodeCoverage(m_tableView->m_contentLayer->getChildren());
    if (this->m_entries->count() > 4)
        m_tableView->m_contentLayer->setContentSize({-coverage.origin.x + coverage.size.width, -coverage.origin.y + coverage.size.height});

    this->m_tableView->moveToTop();

    if (this->m_entries->count() == 1)
        this->m_tableView->moveToTopWithOffset(this->m_itemSeparation);
}

void DailyListView::loadCell(TableViewCell* cell, int index) {
    static_cast<DailyCell*>(cell)->loadFromLevel(
        static_cast<GJGameLevel*>(this->m_entries->objectAtIndex(index))
    );
    static_cast<DailyCell*>(cell)->updateBGColor(index);
}

TableViewCell* DailyListView::getListCell(const char* key) {
    return new DailyCell(key, { this->m_width, 55.0f });
}