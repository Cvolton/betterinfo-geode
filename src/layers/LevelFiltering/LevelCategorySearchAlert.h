#pragma once
#include <Geode/Geode.hpp>
#include "../../utils.hpp"

using namespace geode::prelude;

//TODO: use CvoltonAlertLayerStub
class BI_DLL LevelCategorySearchAlert : public FLAlertLayer, public SetIDPopupDelegate {
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