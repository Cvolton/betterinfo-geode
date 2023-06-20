#include "utils.hpp"

void BetterInfo::copyToClipboard(const char* text){
        clipboard::write(text);
}

void BetterInfo::copyToClipboard(const char* text, CCLayer* parent){
        copyToClipboard(text);

        parent->addChild(TextAlertPopup::create("Copied to clipboard", 0.5f, 0.6f), 100);
}

std::string BetterInfo::getSongUrl(int audioID) {
        auto songInfo = static_cast<SongInfoObject*>(MusicDownloadManager::sharedState()->m_songsDict->objectForKey(std::to_string(audioID)));
        if(!songInfo) return "";
        return songInfo->m_songURL;
}

bool BetterInfo::isNewGrounds(int audioID) {
        return BetterInfo::getSongUrl(audioID).find("://audio.ngfiles.com/") != std::string::npos;
}