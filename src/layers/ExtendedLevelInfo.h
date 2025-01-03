#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include "_bases/CvoltonAlertLayerStub.h"
#include "../delegates/UploadDateDelegate.h"
#include "../utils.hpp"

class BI_DLL ExtendedLevelInfo : public CvoltonAlertLayerStub, public UploadDateDelegate {
    std::string m_primary;
    std::string m_secondary;
    std::string m_fileSizeCompressed = "NA";
    std::string m_fileSizeUncompressed = "NA";
    std::string m_maxGameVersion = "NA";
    std::string m_uploadDateEstimated = "NA";
    std::vector<std::string> m_primaryValues;
    std::vector<std::string> m_secondaryValues;
    Ref<GJGameLevel> m_level;
    TextArea* m_info;
    CCMenuItemSpriteExtra* m_prevBtn;
    CCMenuItemSpriteExtra* m_nextBtn;
    int m_page = 0;
    size_t m_objectsEstimated = 0;
public:
    static ExtendedLevelInfo* create(GJGameLevel* level);
    void onClose(cocos2d::CCObject* sender);
    void onCopyName(cocos2d::CCObject* sender);
    void onCopyAuthor(cocos2d::CCObject* sender);
    void onCopyDesc(cocos2d::CCObject* sender);
    void onCopyInfo(cocos2d::CCObject* sender);
    void onNext(cocos2d::CCObject* sender);
    void onPrev(cocos2d::CCObject* sender);
    void onUploadDateLoaded(int levelID, const std::string& date);
    void loadPage(int page);
    void refreshInfoTexts();
    void setupAdditionalInfo();
    bool init(GJGameLevel* level);
    static cocos2d::CCLabelBMFont* createTextLabel(const std::string text, const cocos2d::CCPoint& position, const float scale, cocos2d::CCNode* menu, const char* font = "bigFont.fnt");
    void keyDown(cocos2d::enumKeyCodes key);
};