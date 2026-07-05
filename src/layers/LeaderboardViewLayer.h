#pragma once

#include "../delegates/BILeaderboardDelegate.h"
#include "_bases/BIViewLayer.h"

enum class BILeaderboardMode {
    Account,
    Top1000
};

class BI_DLL LeaderboardViewLayer : public BIViewLayer, public BILeaderboardDelegate {
    BILeaderboardMode m_mode = BILeaderboardMode::Account;
    int m_accountID = 0;
    int m_stat = 0;
    Ref<CCMenu> m_rightMenu = nullptr;
    std::vector<Ref<CCMenuItemSpriteExtra>> m_statBtns;
protected:
    virtual bool init(BILeaderboardMode mode, int accountID);
    virtual void keyBackClicked();
    void setupStatBtns();
    void onRefresh(cocos2d::CCObject*);
    void loadStat(int stat, bool reload);
public:
    virtual void loadPage();
    static LeaderboardViewLayer* create(BILeaderboardMode mode, int accountID);
    static cocos2d::CCScene* scene(BILeaderboardMode mode, int accountID);

    void onLeaderboardFinished(cocos2d::CCArray* scores, int stat);

    ~LeaderboardViewLayer();
};