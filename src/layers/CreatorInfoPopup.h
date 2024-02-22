#pragma once
#include "_bases/CvoltonAlertLayerStub.h"

class CreatorInfoPopup : public CvoltonAlertLayerStub, public LevelManagerDelegate {
    cocos2d::CCArray* m_levels = nullptr;
    LoadingCircle* m_circle = nullptr;
    cocos2d::CCMenu* m_tabMenu = nullptr;
    cocos2d::CCMenu* m_diffMenu = nullptr;
    cocos2d::CCMenu* m_secondRowMenu = nullptr;
    cocos2d::CCMenu* m_thirdRowMenu = nullptr;
    cocos2d::CCMenu* m_cornerMenu = nullptr;
    GJSearchObject* m_searchObject = nullptr;
    cocos2d::CCLabelBMFont* m_classic = nullptr;
    cocos2d::CCLabelBMFont* m_platformer = nullptr;
    int m_tab = 1;
    bool m_loaded = false;
public:
    static CreatorInfoPopup* create(int userID);
    bool init(int userID);
    void onClose(cocos2d::CCObject*);
    void onTab(cocos2d::CCObject*);
    CCMenuItemSpriteExtra* createTab(const char* text, const char* icon, int tab);
    void createTabs();
    void showResults();
    void loadLevels();

    void queueLoad(float dt);

    void loadLevelsFinished(cocos2d::CCArray*, const char*);
    void loadLevelsFailed(const char*);

    int levelsForDifficulty(int difficulty, bool platformer);
};