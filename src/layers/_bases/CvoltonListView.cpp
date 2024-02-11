#include "CvoltonListView.h"
#include <Geode/Geode.hpp>

#include "../DailyHistory/DailyCell.h"
#include "../RewardHistory/RewardCell.h"

using namespace geode::prelude;

template <class Cell>
bool CvoltonListView<Cell>::init(cocos2d::CCArray* entries, float width, float height, float separation) {
    m_itemSeparation = separation;

    if(!CustomListView::init(entries, (BoomListType) 0x420, width, height)) return false;

    m_tableView->retain();
    return true;
}

template <class Cell>
CvoltonListView<Cell>::~CvoltonListView<Cell>() {
    m_tableView->release();
}

template <class Cell>
CvoltonListView<Cell>* CvoltonListView<Cell>::create(cocos2d::CCArray* data, float width, float height, float separation) {
    auto ret = new CvoltonListView<Cell>();
    if (ret && ret->init(data, width, height, separation)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

template <class Cell>
void CvoltonListView<Cell>::setupList(float) {
    this->m_tableView->reloadData();

    auto coverage = calculateNodeCoverage(m_tableView->m_contentLayer->getChildren());
    if (this->m_entries->count() > 4)
        m_tableView->m_contentLayer->setContentSize({-coverage.origin.x + coverage.size.width, -coverage.origin.y + coverage.size.height});

    this->m_tableView->moveToTop();

    if (this->m_entries->count() == 1)
        this->m_tableView->moveToTopWithOffset(this->m_itemSeparation);
}

template <class Cell>
void CvoltonListView<Cell>::loadCell(TableViewCell* cell, int index) {
    static_cast<Cell*>(cell)->loadFromData(
        this->m_entries->objectAtIndex(index)
    );
    static_cast<Cell*>(cell)->updateBGColor(index);
}

template <class Cell>
TableViewCell* CvoltonListView<Cell>::getListCell(const char* key) {
    return new Cell(key, { this->m_width, m_itemSeparation });
}

template class CvoltonListView<DailyCell>;
template class CvoltonListView<RewardCell>;