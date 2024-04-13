#pragma once

#include <Geode/Geode.hpp>
#include "../utils.hpp"

using namespace geode::prelude;

//TODO: use CvoltonAlertLayerStub
class BI_DLL CustomLevelSearchLayer : public FLAlertLayer {
    Ref<GJGameLevel> m_level = nullptr;
public:
    static CustomLevelSearchLayer* create(GJGameLevel* level);
    void onClose(cocos2d::CCObject* sender);
    void onSimilar(cocos2d::CCObject* sender);
    void onSong(cocos2d::CCObject* sender);
    void onNeighbors(cocos2d::CCObject* sender);
    bool init(GJGameLevel* level);
    CCMenuItemSpriteExtra* createButton(cocos2d::CCNode* menu, const char* text, cocos2d::SEL_MenuHandler handler, float x, float y, int width, float height, float scale);
};