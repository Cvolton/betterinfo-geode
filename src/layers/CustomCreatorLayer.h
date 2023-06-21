#pragma once

#include <Geode/Bindings.hpp>

using namespace geode::prelude;

class CustomCreatorLayer : public cocos2d::CCLayer {
protected:
    virtual bool init();
    virtual void keyBackClicked();
    void onBack(cocos2d::CCObject*);
    void onFeatured(cocos2d::CCObject*);
    void onMostLiked(cocos2d::CCObject*);
    void onDaily(cocos2d::CCObject*);
    void onWeekly(cocos2d::CCObject*);
    void onInfo(cocos2d::CCObject*);
    void onSearch(cocos2d::CCObject*);
    void onSearchID(cocos2d::CCObject*);
    void onSettings(cocos2d::CCObject*);
public:
    static CustomCreatorLayer* create();
    static cocos2d::CCScene* scene();
};