#pragma once
#include "_bases/CvoltonAlertLayerStub.h"
#include "../delegates/PageNumberDelegate.h"
#include "../ui/DoubleArrow.h"

class JumpToPageLayer : public CvoltonAlertLayerStub, public LevelCommentDelegate {
    Ref<InfoLayer> m_infoLayer = nullptr;
    PageNumberDelegate* m_pageNumberDelegate = nullptr;
    CCTextInputNode* m_textNode = nullptr;
    CCMenuItemSpriteExtra* m_lastBtn = nullptr;
    DoubleArrow* m_lastArrow = nullptr;
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
    void onLast(cocos2d::CCObject* sender);
    bool init();
    int pageNumber();
    static cocos2d::CCLabelBMFont* createTextLabel(const std::string text, const cocos2d::CCPoint& position, const float scale, cocos2d::CCNode* menu, const char* font = "bigFont.fnt");

    virtual void loadCommentsFinished(cocos2d::CCArray*, char const*);
	virtual void loadCommentsFailed(char const*);
};