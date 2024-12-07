#include "CopyableLabel.h"
#include "../utils.hpp"

bool CopyableLabel::init(const char* color, const char* title, const std::string& value, const char* font) {
    auto area = TextArea::create(fmt::format("<{}>{}:</c> {}", color, title, value), "chatFont.fnt", 1, 200, {0.5,0.5}, 20, false);
    m_value = value;

    if(!CCMenuItemSpriteExtra::init(area, nullptr, this, nullptr)) return false;

    this->setContentSize({200, 20});
    area->setPosition(this->getContentSize() / 2);

    return true;
}

CopyableLabel* CopyableLabel::create(const char* color, const char* title, const std::string& value, const char* font) {
    auto ret = new CopyableLabel();
    if(ret && ret->init(color, title, value, font)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}


void CopyableLabel::activate() {
    CCMenuItemSpriteExtra::activate();

    BetterInfo::copyToClipboard(m_value.c_str(), this);
}