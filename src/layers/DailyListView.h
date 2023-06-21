#pragma once

#include "CvoltonListView.h"

class DailyListView : public CvoltonListView {
protected:
    void setupList() override;
    TableViewCell* getListCell(const char* key) override;
    void loadCell(TableViewCell* cell, int index) override;
    
public:
    static DailyListView* create(cocos2d::CCArray* levels, float width, float height);
};