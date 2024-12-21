#pragma once

#include <Geode/Geode.hpp>

#include "_bases/BIBaseLayer.h"

using namespace geode::prelude;

class BI_DLL CustomCreatorLayer : public BIBaseLayer {
protected:
    virtual bool init();
    void onBack(cocos2d::CCObject*);
    void onBrowserButton(cocos2d::CCObject*);
    void onInfo(cocos2d::CCObject*);
    void onSearch(cocos2d::CCObject*);
    void onSearchID(cocos2d::CCObject*);
    void onSettings(cocos2d::CCObject*);
    void onEgg(cocos2d::CCObject*);
public:
    static CustomCreatorLayer* create();
    static cocos2d::CCScene* scene();
};