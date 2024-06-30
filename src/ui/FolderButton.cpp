#include "FolderButton.h"

bool FolderButton::init(const char* texture, const std::function<void(int)>& callback) {
    if(!CCMenuItemSpriteExtra::init(CCSprite::createWithSpriteFrameName(texture), nullptr, this, nullptr)) return false;

    m_callback = callback;
    m_label = CCLabelBMFont::create("0", "bigFont.fnt");
    m_label->setPosition(this->getNormalImage()->getContentSize() / 2);
    m_label->setScale(.55f);
    this->getNormalImage()->addChild(m_label);

    return true;
}

FolderButton* FolderButton::create(const char* texture, const std::function<void(int)>& callback) {
    auto ret = new FolderButton();
    if(ret && ret->init(texture, callback)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

FolderButton* FolderButton::create(const std::function<void(int)>& callback) {
    return FolderButton::create("gj_folderBtn_001.png", callback);
}

void FolderButton::setDisplayFolder(int folderID) {
    m_label->setString(std::to_string(folderID).c_str());
    m_folderID = folderID;
}

void FolderButton::setIsCreated(bool isCreated) {
    m_isCreated = isCreated;
}

void FolderButton::activate() {
    CCMenuItemSpriteExtra::activate();

    auto popup = SetFolderPopup::create(m_folderID, m_isCreated, m_popupLabel.c_str());
    popup->m_delegate = this;
    popup->show();
}

void FolderButton::setIDPopupClosed(SetIDPopup* popup, int id) {
    if(m_callback) m_callback(id);
    this->setDisplayFolder(id);
}