#pragma once

#include <Geode/Geode.hpp>
#include "../../utils.hpp"

using namespace geode::prelude;

class BI_DLL BIBaseLayer : public cocos2d::CCLayer {
protected:
    virtual bool init(bool BL = true, bool BR = true, bool TL = true, bool TR = true);
    virtual void keyBackClicked();
    void onBack(cocos2d::CCObject*);
public:
    static BIBaseLayer* create(bool BL = true, bool BR = true, bool TL = true, bool TR = true);
    static cocos2d::CCScene* scene(bool BL = true, bool BR = true, bool TL = true, bool TR = true);
    void setCorners(bool BL, bool BR, bool TL, bool TR);
    void onEnterTransitionDidFinish();

    CCSprite* m_cornerBL = nullptr;
    CCSprite* m_cornerBR = nullptr;
    CCSprite* m_cornerTL = nullptr;
    CCSprite* m_cornerTR = nullptr;
};