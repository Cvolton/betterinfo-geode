#pragma once

#include "../_bases/BIViewLayer.h"
#include "DailyCell.h"
#include "../../delegates/PageNumberDelegate.h"

class DailyViewLayer : public BIViewLayer {
    bool m_isWeekly = false;
protected:
    virtual bool init(bool isWeekly);
    void onMore(cocos2d::CCObject*);
public:
    virtual void loadPage();
    static DailyViewLayer* create(bool isWeekly);
    static bool compareDailies(const void* l1, const void* l2);
    static cocos2d::CCScene* scene(bool isWeekly);
};