#include "utils.hpp"

void BetterInfo::copyToClipboard(const char* text){
        clipboard::write(text);
}

void BetterInfo::copyToClipboard(const char* text, CCLayer* parent){
        copyToClipboard(text);

        parent->addChild(TextAlertPopup::create("Copied to clipboard", 0.5f, 0.6f), 100);
}