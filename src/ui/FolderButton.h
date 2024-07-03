#pragma once
#include <Geode/Geode.hpp>
#include "../utils.hpp"

using namespace geode::prelude;

class BI_DLL FolderButton : public CCMenuItemSpriteExtra, public SetIDPopupDelegate {
    std::function<void(int)> m_callback;
    CCLabelBMFont* m_label = nullptr;
    std::string m_popupLabel = "Set Folder";
    int m_folderID = 0;
    bool m_isCreated = false;
protected:
    bool init(const char* texture, const std::function<void(int)>& callback);
public:
    static FolderButton* create(const char* texture, const std::function<void(int)>& callback);
    static FolderButton* create(const std::function<void(int)>& callback);
    void setDisplayFolder(int folderID);
    void setIsCreated(bool isCreated);
    void setPopupLabel(std::string_view label);
    void activate();
    void setIDPopupClosed(SetIDPopup*, int);
};