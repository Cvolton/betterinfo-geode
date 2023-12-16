#pragma once

#include "../_bases/CvoltonListView.h"
#include "RewardCell.h"
#include "../../delegates/PageNumberDelegate.h"

class RewardViewLayer : public cocos2d::CCLayer, public PageNumberDelegate {
    GJListLayer* m_listLayer = nullptr;
    cocos2d::CCArray* m_sortedRewards = nullptr;
    CCMenuItemSpriteExtra* m_prevBtn = nullptr;
    CCMenuItemSpriteExtra* m_nextBtn = nullptr;
    cocos2d::CCLabelBMFont* m_counter = nullptr;
    ButtonSprite* m_pageBtnSprite = nullptr;
    const char* m_title = nullptr;
    unsigned int m_page = 0;
protected:
    virtual bool init(cocos2d::CCDictionary* chests, const char* title);
    virtual void keyBackClicked();
    void onBack(cocos2d::CCObject*);
    void onPrev(cocos2d::CCObject*);
    void onNext(cocos2d::CCObject*);
    void onJumpToPageLayer(cocos2d::CCObject*);
    void onRandom(cocos2d::CCObject*);
    int rewardsPerPage() const;
public:
    virtual void loadPage(unsigned int page);
    static RewardViewLayer* create(cocos2d::CCDictionary* chests, const char* title);
    static bool compareRewards(const void* l1, const void* l2);
    static cocos2d::CCScene* scene(cocos2d::CCDictionary* chests, const char* title);
    virtual int getPage() const;
    void keyDown(cocos2d::enumKeyCodes key);
};