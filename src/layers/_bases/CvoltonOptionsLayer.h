#pragma once
#include "CvoltonAlertLayerStub.h"

#include "../../utils.hpp"

class BI_DLL CvoltonOptionsLayer : public CvoltonAlertLayerStub {
protected:
    int m_toggleCount = 0;
    std::vector<CCNode*> m_toggles;

    int m_columns = 1;
    float m_top = 90;
    float m_gap = 40;
public:
    static CvoltonOptionsLayer* create();
    void onClose(cocos2d::CCObject* sender);
    virtual void onToggle(cocos2d::CCObject* sender);
    bool init();
    virtual void destroyToggles();
    virtual void drawToggles();
    static cocos2d::CCLabelBMFont* createTextLabel(const std::string text, const cocos2d::CCPoint& position, const float scale, cocos2d::CCNode* menu, const char* font = "bigFont.fnt");
    void createToggle(const char* option, const char* name, cocos2d::SEL_MenuHandler additional = nullptr, CCNode* parent = nullptr);
    void createToggle(const char* option, const char* name, float x, float y, cocos2d::SEL_MenuHandler additional = nullptr, CCNode* parent = nullptr);
    void createButtonToggle(const char* option, cocos2d::CCNode* sprite, float x, float y, float scale = 1.f, CCNode* parent = nullptr);

    virtual bool getOption(const std::string& option) const;
    virtual int getOptionInt(const std::string& option) const;
    virtual bool toggleOption(const std::string& option);
    virtual void setOption(const std::string& option, bool value);
    virtual void setOptionInt(const std::string& option, int value);
};