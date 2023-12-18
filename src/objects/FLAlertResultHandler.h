#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class FLAlertResultHandler : public cocos2d::CCNode, public FLAlertLayerProtocol {
    InfoLayer* m_layer = nullptr;
    const char* m_setting = nullptr;
    bool m_refreshMessage = false;
public:
    static FLAlertResultHandler* create(const char* setting, bool refreshMessage = false);
    void FLAlert_Clicked(FLAlertLayer*, bool);
    bool init(const char* setting, bool refreshMessage = false);
};