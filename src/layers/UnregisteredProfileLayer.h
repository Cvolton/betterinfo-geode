#pragma once
#include "_bases/CvoltonAlertLayerStub.h"

class BI_DLL UnregisteredProfileLayer : public CvoltonAlertLayerStub {
    Ref<GJUserScore> m_score = nullptr;
    Ref<CCNode> m_invoker = nullptr;
public:
    static UnregisteredProfileLayer* create(GJUserScore* score, CCNode* invoker = nullptr);
    void onClose(CCObject* sender);
    static void displayProfile(int userID, gd::string userName, CCNode* invoker = nullptr);
    static void doSearch(int userID);
    void onMyLevels(CCObject* sender);
    void onCommentHistory(CCObject* sender);
    void onCopyUserID(CCObject* sender);
    void onCopyPlayerName(CCObject* sender);
    bool init(GJUserScore* score, CCNode* invoker = nullptr);
};