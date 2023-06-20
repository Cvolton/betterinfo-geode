#include "FLAlertResultHandler.h"

FLAlertResultHandler* FLAlertResultHandler::create(){
    auto ret = new FLAlertResultHandler();
    if (ret && ret->init()) {
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
        Mod::get()->setSettingValue<bool>("show-comment-ids", true);

        FLAlertLayer::create(Mod::get()->getName().c_str(), "Refresh for the change to take effect.", "OK")->show();
    }
}

bool FLAlertResultHandler::init(){
    return CCNode::init();
}