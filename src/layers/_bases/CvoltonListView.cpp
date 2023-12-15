#include "CvoltonListView.h"

using namespace gd;
using namespace cocos2d;

bool CvoltonListView::init(cocos2d::CCArray* entries, int btype, float width, float height) {
    if(!CustomListView::init(entries, (BoomListType) btype, width, height)) return false;

    m_tableView->retain();
    return true;
}

CvoltonListView::~CvoltonListView() {
    m_tableView->release();
}