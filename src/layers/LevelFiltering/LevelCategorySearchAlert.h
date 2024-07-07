#pragma once
#include <Geode/Geode.hpp>
#include "../../utils.hpp"
#include "../_bases/CvoltonAlertLayerStub.h"

using namespace geode::prelude;

class BI_DLL LevelCategorySearchAlert : public CvoltonAlertLayerStub, public SetIDPopupDelegate {
public:
    static LevelCategorySearchAlert* create();
    void onClose(cocos2d::CCObject* sender);
    void onPlayed(cocos2d::CCObject* sender);
    void onFavorites(cocos2d::CCObject* sender);
    void onFolder(cocos2d::CCObject* sender);
    void onCompleted(cocos2d::CCObject* sender);
    void onOrbs(cocos2d::CCObject* sender);
    void onLeaderboard(cocos2d::CCObject* sender);
    void onCoins(cocos2d::CCObject* sender);
    void onNoCoins(cocos2d::CCObject* sender);
    void setIDPopupClosed(SetIDPopup*, int);
    bool init();
    CCMenuItemSpriteExtra* createButton(cocos2d::CCNode* menu, const char* text, cocos2d::SEL_MenuHandler handler, float x, float y, int width, float height, float scale);
};