#pragma once

#include <Geode/Bindings.hpp>

using namespace geode::prelude;

class BIBaseLayer : public cocos2d::CCLayer {
protected:
    virtual bool init();
    virtual void keyBackClicked();
    void onBack(cocos2d::CCObject*);
public:
    static BIBaseLayer* create();
    static cocos2d::CCScene* scene();
};