#pragma once
#include <Geode/Geode.hpp>
#include "../utils.hpp"

using namespace geode::prelude;

class BI_DLL CopyableLabel : public CCMenuItemSpriteExtra {
    std::string m_value;
protected:
    bool init(const char* color, const char* title, const std::string& value, const char* font = "charFont.fnt");
public:
    static CopyableLabel* create(const char* color, const char* title, const std::string& value, const char* font = "charFont.fnt");
    void activate();
};