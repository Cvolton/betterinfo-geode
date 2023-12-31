#pragma once
#include "_bases/CvoltonAlertLayerStub.h"
#include "DailyHistory/DailyViewLayer.h"

class JumpToPageLayer : public CvoltonAlertLayerStub {
    InfoLayer* m_infoLayer = nullptr;
    PageNumberDelegate* m_pageNumberDelegate = nullptr;
    CCTextInputNode* m_textNode;
public:
    static JumpToPageLayer* create(InfoLayer* infoLayer);
    static JumpToPageLayer* create(PageNumberDelegate* pageNumberDelegate);
    bool init(InfoLayer* infoLayer);
    bool init(PageNumberDelegate* pageNumberDelegate);
    void onClose(cocos2d::CCObject* sender);
    void onOK(cocos2d::CCObject* sender);
    void onPrev(cocos2d::CCObject* sender);
    void onNext(cocos2d::CCObject* sender);
    void onReset(cocos2d::CCObject* sender);
    bool init();
    int pageNumber();
    static cocos2d::CCLabelBMFont* createTextLabel(const std::string text, const cocos2d::CCPoint& position, const float scale, cocos2d::CCNode* menu, const char* font = "bigFont.fnt");
};