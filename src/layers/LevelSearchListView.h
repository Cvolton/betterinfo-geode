#pragma once

#include "CvoltonListView.h"
#include <Geode/Bindings.hpp>

using namespace geode::prelude;

class LevelSearchListView : public CvoltonListView {
protected:
    void setupList() override;
    TableViewCell* getListCell(const char* key) override;
    void loadCell(TableViewCell* cell, int index) override;
    
public:
    static LevelSearchListView* create(cocos2d::CCArray* levels, float width, float height);
};