#pragma once

#include <Geode/Geode.hpp>

/**
 * @brief      This class only exists as a bugfix to CustomListView
 */
class CvoltonListView : public CustomListView {
protected:
    bool init(cocos2d::CCArray* entries, int btype, float width, float height);
    virtual ~CvoltonListView();
};