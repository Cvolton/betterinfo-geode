#pragma once
#include "_bases/CvoltonAlertLayerStub.h"
#include <unordered_set>

class BI_DLL CreatorInfoPopup : public CvoltonAlertLayerStub, public LevelManagerDelegate {
    Ref<cocos2d::CCArray> m_levels = nullptr;
    LoadingCircle* m_circle = nullptr;
    cocos2d::CCMenu* m_tabMenu = nullptr;
    cocos2d::CCMenu* m_diffMenu = nullptr;
    cocos2d::CCMenu* m_secondRowMenu = nullptr;
    cocos2d::CCMenu* m_thirdRowMenu = nullptr;
    cocos2d::CCMenu* m_cornerMenu = nullptr;
    Ref<GJSearchObject> m_searchObject = nullptr;
    cocos2d::CCLabelBMFont* m_classic = nullptr;
    cocos2d::CCLabelBMFont* m_platformer = nullptr;
    CCMenuItemSpriteExtra* m_currentTab = nullptr;
    cocos2d::CCLabelBMFont* m_loading = nullptr;
    std::unordered_set<int> m_loadedLevelIDs;
    int m_tab = 1;
    int m_totalLevels = -1;
    int m_loadedPageEnd = 0;
    bool m_loaded = false;
    bool m_closed = false;
public:
    static CreatorInfoPopup* create(int userID);
    bool init(int userID);
    void onClose(cocos2d::CCObject*);
    void onTab(cocos2d::CCObject*);
    CCMenuItemSpriteExtra* createTab(const char* text, const char* icon, int tab, bool current = false);
    void createTabs();
    void showResults();
    void loadLevels();
    bool shouldFinishLoading(cocos2d::CCArray* levels, int newLevels);

    void queueLoad(float dt);

    void loadLevelsFinished(cocos2d::CCArray*, const char*);
    void loadLevelsFailed(const char*);
    void setupPageInfo(gd::string counts, const char* key);

    int levelsForDifficulty(int difficulty, bool platformer);

    ~CreatorInfoPopup();
};
