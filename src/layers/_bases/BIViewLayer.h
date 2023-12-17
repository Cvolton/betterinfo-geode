#pragma once

#include "CvoltonListView.h"
#include "BIBaseLayer.h"
#include "../../delegates/PageNumberDelegate.h"

class BIViewLayer : public BIBaseLayer, public PageNumberDelegate {
protected:
    std::string m_title;
    BoomListView* m_listView = nullptr;
    GJListLayer* m_listLayer = nullptr;
    cocos2d::CCArray* m_data = nullptr;
    CCMenuItemSpriteExtra* m_prevBtn = nullptr;
    CCMenuItemSpriteExtra* m_nextBtn = nullptr;
    CCMenuItemSpriteExtra* m_pageBtn = nullptr;
    CCMenuItemSpriteExtra* m_randomBtn = nullptr;
    CCMenuItemSpriteExtra* m_firstBtn = nullptr;
    CCMenuItemSpriteExtra* m_lastBtn = nullptr;
    cocos2d::CCLabelBMFont* m_counter = nullptr;
    ButtonSprite* m_pageBtnSprite = nullptr;
    LoadingCircle* m_circle = nullptr;
    unsigned int m_page = 0;
    bool m_paginated = true;

    virtual bool init(bool paginated = true);
    virtual void keyBackClicked();
    virtual void updateCounter();
    void onBack(cocos2d::CCObject*);
    void onPrev(cocos2d::CCObject*);
    void onNext(cocos2d::CCObject*);
    void onFirst(cocos2d::CCObject*);
    void onLast(cocos2d::CCObject*);
    void onJumpToPageLayer(cocos2d::CCObject*);
    void onRandom(cocos2d::CCObject*);
    virtual int resultsPerPage() const;
    CCArray* trimData();
public:
    virtual void loadPage();
    void loadPage(unsigned int page);
    static BIViewLayer* create(bool paginated = true);
    static cocos2d::CCScene* scene(bool paginated = true);
    virtual int getPage() const;
    virtual void keyDown(cocos2d::enumKeyCodes key);
    void setData(cocos2d::CCArray* data);

    void showCircle();
    void hideCircle();
};