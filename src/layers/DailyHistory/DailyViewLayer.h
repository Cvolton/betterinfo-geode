#pragma once

#include "../_bases/BIViewLayer.h"
#include "DailyCell.h"
#include "../../delegates/PageNumberDelegate.h"

class DailyViewLayer : public BIViewLayer {
    GJTimedLevelType m_timedType = GJTimedLevelType::Daily;
protected:
    virtual bool init(GJTimedLevelType timedType);
    void onMore(cocos2d::CCObject*);
    GJSearchObject* getSearchObject();
public:
    virtual void loadPage();
    static DailyViewLayer* create(GJTimedLevelType timedType);
    static bool compareDailies(const void* l1, const void* l2);
    static cocos2d::CCScene* scene(GJTimedLevelType timedType);
};