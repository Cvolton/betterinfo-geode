#pragma once

#include "../delegates/BILeaderboardDelegate.h"
#include "_bases/BIViewLayer.h"

enum class BILeaderboardMode {
    Account,
    Top1000
};

enum class BILeaderboardTab {
    Global,
    Creator,
    Friends
};

class BI_DLL LeaderboardViewLayer : public BIViewLayer, public BILeaderboardDelegate, public UserListDelegate {
    BILeaderboardMode m_mode = BILeaderboardMode::Account;
    BILeaderboardTab m_tab = BILeaderboardTab::Global;
    int m_accountID = 0;
    int m_stat = 0;
    Ref<CCMenu> m_rightMenu = nullptr;
    Ref<CCMenu> m_topMenu = nullptr;
    Ref<CCClippingNode> m_tabsClipper = nullptr;
    Ref<CCSprite> m_tabsGradientSpr = nullptr;
    Ref<CCSprite> m_tabsGradientStencil = nullptr;
    std::vector<Ref<CCMenuItemSpriteExtra>> m_statBtns;
    std::vector<Ref<TabButton>> m_topBtns;
protected:
    virtual bool init(BILeaderboardMode mode, int accountID);
    virtual void keyBackClicked();
    void setupStatBtns();
    void onRefresh(cocos2d::CCObject*);
    void onTabChanged(cocos2d::CCObject*);
    void loadFriends(int stat, bool reload);
    void loadStat(int stat, bool reload);
public:
    virtual void loadPage();
    static LeaderboardViewLayer* create(BILeaderboardMode mode, int accountID);
    static cocos2d::CCScene* scene(BILeaderboardMode mode, int accountID);

    void onLeaderboardFinished(cocos2d::CCArray* scores, int stat);

    ~LeaderboardViewLayer();

    virtual void getUserListFinished(cocos2d::CCArray* scores, UserListType type);
    virtual void getUserListFailed(UserListType type, GJErrorCode errorType);
};