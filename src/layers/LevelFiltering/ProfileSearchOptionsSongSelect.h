#pragma once
#include "../_bases/CvoltonOptionsLayer.h"
#include "../../delegates/SongDialogCloseDelegate.h"

class ProfileSearchOptionsSongSelect : public CvoltonOptionsLayer, public GJDropDownLayerDelegate {
    SongDialogCloseDelegate* m_delegate;
    CCTextInputNode* m_textNode = nullptr;
    CCMenuItemSpriteExtra* m_savedBtn = nullptr;
public:
    static ProfileSearchOptionsSongSelect* create(SongDialogCloseDelegate* delegate);
    void onClose(cocos2d::CCObject* sender);
    bool init(SongDialogCloseDelegate* delegate);
    void destroyToggles();
    void drawToggles();
    int songID();
    void createToggle(const char* option, const char* name, float x, float y);
    void onSaved(CCObject* sender);

    virtual void dropDownLayerWillClose(GJDropDownLayer*);
};