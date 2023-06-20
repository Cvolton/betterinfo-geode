#pragma once

#include "CvoltonListView.h"
#include <Geode/Bindings.hpp>

using namespace geode::prelude;

class LeaderboardListView : public CvoltonListView {
protected:
    virtual void setupList() override;
    TableViewCell* getListCell(const char* key) override;
    virtual void loadCell(TableViewCell* cell, int index) override;
    
public:
    static LeaderboardListView* create(cocos2d::CCArray* scores, float width, float height);
};