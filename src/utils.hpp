#pragma once

#include <Geode/utils/general.hpp>
using namespace geode::prelude;

namespace BetterInfo {
    
    void copyToClipboard(const char* text);
    void copyToClipboard(const char* text, CCLayer* parent);

    std::string getSongUrl(int audioID);
    bool isNewGrounds(int audioID);
}