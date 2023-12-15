#pragma once

#include "DailyListView.h"
#include "../../delegates/PageNumberDelegate.h"

class DailyViewLayer : public cocos2d::CCLayer, public PageNumberDelegate {
    DailyListView* dailyView = nullptr;
    GJListLayer* listLayer = nullptr;
    cocos2d::CCArray* sortedLevels = nullptr;
    CCMenuItemSpriteExtra* prevBtn = nullptr;
    CCMenuItemSpriteExtra* nextBtn = nullptr;
    cocos2d::CCLabelBMFont* counter = nullptr;
    ButtonSprite* pageBtnSprite = nullptr;
    unsigned int page = 0;
    bool isWeekly = false;
protected:
    virtual bool init(bool isWeekly);
    virtual void keyBackClicked();
    void onBack(cocos2d::CCObject*);
    void onPrev(cocos2d::CCObject*);
    void onNext(cocos2d::CCObject*);
    void onJumpToPageLayer(cocos2d::CCObject*);
    void onRandom(cocos2d::CCObject*);
    void onMore(cocos2d::CCObject*);
    int levelsPerPage() const;
public:
    virtual void loadPage(unsigned int page);
    static DailyViewLayer* create(bool isWeekly);
    static bool compareDailies(const void* l1, const void* l2);
    static cocos2d::CCScene* scene(bool isWeekly);
    virtual int getPage() const;
    void keyDown(cocos2d::enumKeyCodes key);
};