#include "FLAlertResultHandler.h"

FLAlertResultHandler* FLAlertResultHandler::create(const char* setting, bool refreshMessage){
    auto ret = new FLAlertResultHandler();
    if (ret && ret->init(setting, refreshMessage)) {
        //robert 1 :D
        ret->autorelease();
        ret->retain();
    } else {
        //robert -1
        delete ret;
        ret = nullptr;
    }
    return ret;
}

void FLAlertResultHandler::FLAlert_Clicked(FLAlertLayer*, bool success) {
    if(success) {
        Mod::get()->setSettingValue<bool>(m_setting, true);

        FLAlertLayer::create(Mod::get()->getName().c_str(), m_refreshMessage ? "Refresh for the change to take effect." : "Setting succesfully applied.", "OK")->show();
    }
}

bool FLAlertResultHandler::init(const char* setting, bool refreshMessage){
    m_setting = setting;
    m_refreshMessage = refreshMessage;

    return CCNode::init();
}