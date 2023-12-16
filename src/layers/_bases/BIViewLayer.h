#pragma once

#include "CvoltonListView.h"
#include "BIBaseLayer.h"
#include "../../delegates/PageNumberDelegate.h"

class BIViewLayer : public BIBaseLayer, public PageNumberDelegate {
protected:
    const char* m_title = nullptr;
    BoomListView* m_listView = nullptr;
    GJListLayer* m_listLayer = nullptr;
    cocos2d::CCArray* m_data = nullptr;
    CCMenuItemSpriteExtra* m_prevBtn = nullptr;
    CCMenuItemSpriteExtra* m_nextBtn = nullptr;
    cocos2d::CCLabelBMFont* m_counter = nullptr;
    ButtonSprite* m_pageBtnSprite = nullptr;
    unsigned int m_page = 0;

    virtual bool init();
    virtual void keyBackClicked();
    void onBack(cocos2d::CCObject*);
    void onPrev(cocos2d::CCObject*);
    void onNext(cocos2d::CCObject*);
    void onJumpToPageLayer(cocos2d::CCObject*);
    void onRandom(cocos2d::CCObject*);
    virtual int resultsPerPage() const;
public:
    virtual void loadPage(unsigned int page);
    static BIViewLayer* create();
    static cocos2d::CCScene* scene();
    virtual int getPage() const;
    virtual void keyDown(cocos2d::enumKeyCodes key);
};