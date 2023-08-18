#include "RewardListView.h"
#include "RewardCell.h"

RewardListView* RewardListView::create(cocos2d::CCArray* rewards, float width, float height) {
    auto ret = new RewardListView();
    if (ret && ret->init(rewards, 0x420, width, height)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

void RewardListView::setupList() {
    this->m_itemSeparation = 45.0f;

    this->m_tableView->reloadData();

    auto coverage = calculateNodeCoverage(m_tableView->m_contentLayer->getChildren());
    if (this->m_entries->count() > 4)
        m_tableView->m_contentLayer->setContentSize({-coverage.origin.x + coverage.size.width, -coverage.origin.y + coverage.size.height});

    this->m_tableView->moveToTop();

    if (this->m_entries->count() == 1)
        this->m_tableView->moveToTopWithOffset(this->m_itemSeparation);
}

void RewardListView::loadCell(TableViewCell* cell, int index) {
    static_cast<RewardCell*>(cell)->loadFromReward(
        static_cast<GJRewardItem*>(this->m_entries->objectAtIndex(index))
    );
    static_cast<StatsCell*>(cell)->updateBGColor(index);
}

TableViewCell* RewardListView::getListCell(const char* key) {
    return new RewardCell(key, { this->m_width, this->m_itemSeparation });
}