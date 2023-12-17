#pragma once

#include "../delegates/BILeaderboardDelegate.h"
#include "_bases/BIViewLayer.h"

class LeaderboardViewLayer : public BIViewLayer, public BILeaderboardDelegate {
    LoadingCircle* m_circle = nullptr;
    int m_accountID = 0;
protected:
    virtual bool init(int accountID);
    virtual void keyBackClicked();
    void onRefresh(cocos2d::CCObject*);
public:
    virtual void loadPage();
    static LeaderboardViewLayer* create(int accountID);
    static cocos2d::CCScene* scene(int accountID);

    void onLeaderboardFinished(cocos2d::CCArray* scores);
};