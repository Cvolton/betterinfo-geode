#pragma once

#include "../delegates/BILeaderboardDelegate.h"
#include "_bases/BIViewLayer.h"

class BI_DLL LeaderboardViewLayer : public BIViewLayer, public BILeaderboardDelegate {
    int m_accountID = 0;
    int m_stat = 0;
    Ref<CCMenu> m_rightMenu = nullptr;
    std::vector<Ref<CCMenuItemSpriteExtra>> m_statBtns;
protected:
    virtual bool init(int accountID);
    virtual void keyBackClicked();
    void setupStatBtns();
    void onRefresh(cocos2d::CCObject*);
    void loadStat(int stat);
public:
    virtual void loadPage();
    static LeaderboardViewLayer* create(int accountID);
    static cocos2d::CCScene* scene(int accountID);

    void onLeaderboardFinished(cocos2d::CCArray* scores);

    ~LeaderboardViewLayer();
};