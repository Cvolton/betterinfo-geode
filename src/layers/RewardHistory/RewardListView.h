#pragma once

#include "../_bases/CvoltonListView.h"

class RewardListView : public CvoltonListView {
protected:
    void setupList() override;
    TableViewCell* getListCell(const char* key) override;
    void loadCell(TableViewCell* cell, int index) override;
    
public:
    static RewardListView* create(cocos2d::CCArray* rewards, float width, float height);
};