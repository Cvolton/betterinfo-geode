#include "CvoltonSearchSettings.h"

CvoltonSearchSettings* CvoltonSearchSettings::create(){
    auto ret = new CvoltonSearchSettings();
    if (ret && ret->init()) {
        //robert 1 :D
        ret->autorelease();
    } else {
        //robert -1
        delete ret;
        ret = nullptr;
    }
    return ret;
}

void CvoltonSearchSettings::onClose(cocos2d::CCObject* sender)
{
    destroyToggles();
    CvoltonAlertLayerStub::onClose(sender);
}

bool CvoltonSearchSettings::init(){
    if(!CvoltonAlertLayerStub::init({290.0f, 194.0f}, 1.f, 0x96, 2)) return false;
    m_closeBtn->setPositionX(m_closeBtn->getPositionX() + 0.5f);

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto label = createTextLabel("Search Settings", {(winSize.width / 2), (winSize.height / 2) + 74}, .7f, m_mainLayer, "goldFont.fnt");
    label->setID("title"_spr);

    drawToggles();

    return true;
}

void CvoltonSearchSettings::drawToggles(){
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    createToggle("search_trim", "Trim Spaces", -110, 40);
}