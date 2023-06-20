#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class FLAlertResultHandler : public cocos2d::CCNode, public FLAlertLayerProtocol {
    InfoLayer* m_layer = nullptr;
public:
    static FLAlertResultHandler* create();
    void FLAlert_Clicked(FLAlertLayer*, bool);
    bool init();
};