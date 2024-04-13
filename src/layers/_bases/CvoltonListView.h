#pragma once

#include <Geode/Geode.hpp>
#include "../../utils.hpp"

template <class Cell>
class BI_DLL CvoltonListView : public CustomListView {
protected:
    bool init(cocos2d::CCArray* entries, float width, float height, float separation);
    virtual ~CvoltonListView();

    void setupList(float) override;
    TableViewCell* getListCell(const char* key) override;
    void loadCell(TableViewCell* cell, int index) override;
    
public:
    static CvoltonListView<Cell>* create(cocos2d::CCArray* data, float width, float height, float separation = 55.0f);
};