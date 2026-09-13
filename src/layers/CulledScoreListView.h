#pragma once

#include <Geode/Geode.hpp>
#include "_bases/CvoltonListView.h"
#include "CulledScoreCell.h"

class BI_DLL CulledScoreListView : public CvoltonListView<CulledScoreCell> {
protected:
    void setupList(float) override;
    
public:
    static CulledScoreListView* create(cocos2d::CCArray* data, float width, float height, float separation = 55.0f);
};