#pragma once
#include "_bases/CvoltonAlertLayerStub.h"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class BI_DLL LevelBrowserEndLayer : public CvoltonAlertLayerStub, public LevelManagerDelegate, public LevelCommentDelegate {
    Ref<LevelBrowserLayer> m_levelBrowserLayer = nullptr;
    Ref<InfoLayer> m_infoLayer = nullptr;
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
    static LevelBrowserEndLayer* create(LevelBrowserLayer* levelBrowserLayer, InfoLayer* infoLayer);
    void onClose(cocos2d::CCObject* sender);
    void onOK(cocos2d::CCObject* sender);
    bool init(LevelBrowserLayer* levelBrowserLayer, InfoLayer* infoLayer);
    static cocos2d::CCLabelBMFont* createTextLabel(const std::string text, const cocos2d::CCPoint& position, const float scale, cocos2d::CCNode* menu, const char* font = "bigFont.fnt");

    void loadLevelsFinished(cocos2d::CCArray*, const char*);
    void loadLevelsFinished(cocos2d::CCArray*, const char*, int);
    void loadLevelsFailed(const char*);
    void loadLevelsFailed(const char*, int);
    void setupPageInfo(gd::string, const char*);

    void loadCommentsFinished(cocos2d::CCArray*, char const*);
	void loadCommentsFailed(char const*);

    void updateDisplay();
    void onTimer(float dt);
    void getOnlineLevels();
    void onQueueDownload(float dt);

    ~LevelBrowserEndLayer();
};