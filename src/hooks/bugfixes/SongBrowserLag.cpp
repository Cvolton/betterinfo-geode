//TODO: MusicDownloadManager

/*#include <Geode/Bindings.hpp>
#include <Geode/modify/MusicDownloadManager.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/CustomSongWidget.hpp>

#include "../../managers/BetterInfoCache.h"

using namespace geode::prelude;

class $modify(BIMusicDownloadManager, MusicDownloadManager) {
    bool isSongDownloaded(int songID) {
        auto BICache = BetterInfoCache::sharedState();
        if(BICache->m_downloadedSongs.contains(songID)) {
            return BICache->m_downloadedSongs[songID];
        }

        BICache->m_downloadedSongs[songID] = MusicDownloadManager::isSongDownloaded(songID);
        return BICache->m_downloadedSongs[songID];
    }
};

class $modify(BISBLLevelInfoLayer, LevelInfoLayer) {
    void onPlay(CCObject* sender) {
        auto BICache = BetterInfoCache::sharedState();
        if(BICache->m_downloadedSongs.contains(this->m_level->m_songID)) BICache->m_downloadedSongs.erase(this->m_level->m_songID);

        LevelInfoLayer::onPlay(sender);
    }

    bool init(GJGameLevel* level) {
        auto BICache = BetterInfoCache::sharedState();
        if(BICache->m_downloadedSongs.contains(level->m_songID)) BICache->m_downloadedSongs.erase(level->m_songID);

        return LevelInfoLayer::init(level);
    }
};

class $modify(CustomSongWidget) {
    void updateSongObject(SongInfoObject* song) {
        auto BICache = BetterInfoCache::sharedState();
        if(BICache->m_downloadedSongs.contains(song->m_songID)) BICache->m_downloadedSongs.erase(song->m_songID);

        CustomSongWidget::updateSongObject(song);
    }
};*/