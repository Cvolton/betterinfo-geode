#include "IDRangePopup.h"

#include "../../utils.hpp"

IDRangePopup* IDRangePopup::create(IDRangeDelegate* delegate, int min, int max, const char* text, int additional, bool single){
    auto ret = new IDRangePopup();
    if (ret && ret->init(delegate, min, max, text, additional, single)) {
        //robert 1 :D
        ret->autorelease();
    } else {
        //robert -1
        delete ret;
        ret = nullptr;
    }
    return ret;
}

void IDRangePopup::onClose(cocos2d::CCObject* sender)
{
    if(m_delegate) m_delegate->onIDRangeFinished(minID(), maxID(), m_additional);
    
    CvoltonAlertLayerStub::onClose(sender);
}

bool IDRangePopup::init(IDRangeDelegate* delegate, int min, int max, const char* text, int additional, bool single){
    if(!CvoltonAlertLayerStub::init({240.0f, 150.0f}, .8f, 0x96)) return false;

    this->m_delegate = delegate;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    createTitle(text, .45f, .9f);

    m_minNode = CCTextInputNode::create(100, 30, single ? "Value" : "Min", "bigFont.fnt");
    m_minNode->setLabelPlaceholderColor({0x75, 0xAA, 0xF0});
    if(min != 0) m_minNode->setString(std::to_string(min).c_str());
    m_minNode->setAllowedChars("0123456789");
    m_minNode->setMaxLabelScale(0.7f);
    m_minNode->setMaxLabelWidth(90);
    m_minNode->setPosition({0,6});
    m_minNode->setID("min-input"_spr);
    m_buttonMenu->addChild(m_minNode);

    m_maxNode = CCTextInputNode::create(100, 30, "Max", "bigFont.fnt");
    m_maxNode->setLabelPlaceholderColor({0x75, 0xAA, 0xF0});
    if(max != 0) m_maxNode->setString(std::to_string(max).c_str());
    m_maxNode->setAllowedChars("0123456789");
    m_maxNode->setMaxLabelScale(0.7f);
    m_maxNode->setMaxLabelWidth(90);
    m_maxNode->setPosition({0,-37});
    m_maxNode->setID("max-input"_spr);
    m_buttonMenu->addChild(m_maxNode);

    auto minBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f });
    minBg->setContentSize({200,60});
    minBg->setColor({123,60,31});
    m_buttonMenu->addChild(minBg, -1);
    minBg->setPosition({0,6});
    minBg->setID("min-background"_spr);
    minBg->setScale(0.6f);

    auto maxBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f });
    maxBg->setContentSize({200,60});
    maxBg->setColor({123,60,31});
    m_buttonMenu->addChild(maxBg, -1);
    maxBg->setPosition({0,-37});
    maxBg->setID("max-background"_spr);
    maxBg->setScale(0.6f);
    //createTextLabel("Advanced Options", {(winSize.width / 2), (winSize.height / 2) + 125}, 1.f, m_pLayer, "bigFont.fnt");
    this->m_additional = additional;

    if(single) {
        maxBg->setVisible(false);
        m_maxNode->setVisible(false);
        minBg->setPositionY((minBg->getPositionY() + maxBg->getPositionY()) / 2);
        m_minNode->setPositionY((m_minNode->getPositionY() + m_maxNode->getPositionY()) / 2);
    }

    return true;
}

int IDRangePopup::minID(){
    return BetterInfo::stoi(m_minNode->getString());
}

int IDRangePopup::maxID(){
    return BetterInfo::stoi(m_maxNode->getString());
}