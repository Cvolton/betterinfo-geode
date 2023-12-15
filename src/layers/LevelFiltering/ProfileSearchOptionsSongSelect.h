#pragma once
#include "../_bases/CvoltonOptionsLayer.h"
#include "../../delegates/DialogCloseDelegate.h"

class ProfileSearchOptionsSongSelect : public CvoltonOptionsLayer {
    DialogCloseDelegate* delegate;
    CCTextInputNode* textNode = nullptr;
public:
    static ProfileSearchOptionsSongSelect* create(DialogCloseDelegate* delegate);
    void onClose(cocos2d::CCObject* sender);
    bool init(DialogCloseDelegate* delegate);
    void destroyToggles();
    void drawToggles();
    int songID();
    void createToggle(const char* option, const char* name, float x, float y);
};