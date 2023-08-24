#pragma once
#include "CvoltonAlertLayerStub.h"
#include <Geode/Bindings.hpp>

using namespace geode::prelude;

class LevelBrowserEndLayer : public CvoltonAlertLayerStub, public OnlineListDelegate {
    LevelBrowserLayer* m_levelBrowserLayer = nullptr;
    TextArea* m_textLabel = nullptr;
    CCMenuItemSpriteExtra* m_goBtn = nullptr;
    LoadingCircle* m_circle = nullptr;
    cocos2d::CCLabelBMFont* m_timer = nullptr;

    int m_min = 0;
    int m_max = 0;
    int m_requests = 0;
    int m_requestsToMax = 0;

    time_t m_maxReached = 0;
    time_t m_lastLoad = 0;

    bool m_updateLabel = false;
public:
    static LevelBrowserEndLayer* create(LevelBrowserLayer* levelBrowserLayer);
    void onClose(cocos2d::CCObject* sender);
    void onOK(cocos2d::CCObject* sender);
    bool init(LevelBrowserLayer* levelBrowserLayer);
    static cocos2d::CCLabelBMFont* createTextLabel(const std::string text, const cocos2d::CCPoint& position, const float scale, cocos2d::CCNode* menu, const char* font = "bigFont.fnt");

    void loadListFinished(cocos2d::CCArray*, const char*);
    void loadListFailed(const char*);
    void setupPageInfo(gd::string, const char*);

    void updateDisplay();
    void onTimer(float dt);
    void getOnlineLevels(GJSearchObject* searchObj);
    void onQueueDownload(float dt);
};