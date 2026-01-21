#pragma once
#include "../_bases/CvoltonOptionsLayer.h"
#include "../../delegates/SongDialogCloseDelegate.h"
#include "../../delegates/IDRangeDelegate.h"
#include "../../delegates/BISearchObjectDelegate.h"
#include "../../objects/BISearchObject.h"
#include "Geode/binding/SetIDPopupDelegate.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class BI_DLL ProfileSearchOptions : public CvoltonOptionsLayer, public SongDialogCloseDelegate, public IDRangeDelegate, public SetIDPopupDelegate {
    std::unordered_map<std::string, bool> m_options;
    std::unordered_map<std::string, int> m_optionInts;
    Ref<LevelBrowserLayer> m_levelBrowserLayer = nullptr;
    CCMenuItemSpriteExtra* m_prevBtn = nullptr;
    CCMenuItemSpriteExtra* m_nextBtn = nullptr;
    cocos2d::extension::CCScale9Sprite* m_lengthBg = nullptr;
    cocos2d::extension::CCScale9Sprite* m_diffBg = nullptr;
    cocos2d::extension::CCScale9Sprite* m_demonDiffBg = nullptr;
    BISearchObjectDelegate* m_searchObjDelegate = nullptr;
    cocos2d::CCMenu* m_lenMenu = nullptr;
    std::string m_prefix;
    int m_page = 0;
public:
    static ProfileSearchOptions* create(LevelBrowserLayer* levelBrowserLayer = nullptr, const std::string& prefix = "", BISearchObjectDelegate* searchObjDelegate = nullptr);
    void onClose(cocos2d::CCObject* sender);
    void onPrev(cocos2d::CCObject* sender);
    void onSong(cocos2d::CCObject* sender);
    void onIdRange(cocos2d::CCObject* sender);
    void onGameVersionRange(cocos2d::CCObject* sender);
    void onFolder(cocos2d::CCObject* sender);
    void onCoins(cocos2d::CCObject* sender);
    void onStarRange(cocos2d::CCObject* sender);
    void onPercentage(cocos2d::CCObject* sender);
    void onPercentageOrbs(cocos2d::CCObject* sender);
    void onPercentageLeaderboard(cocos2d::CCObject* sender);
    void onNext(cocos2d::CCObject* sender);
    void onSecondaryInfo(cocos2d::CCObject* sender);
    bool init(LevelBrowserLayer* levelBrowserLayer = nullptr, const std::string& prefix = "", BISearchObjectDelegate* searchObjDelegate = nullptr);
    void reloadBrowser();
    void drawToggles();
    void drawTogglesPrimary();
    void drawTogglesSecondary();
    void drawTogglesTerciary();
    void onSongDialogClosed(bool custom, int songID);
    void onIDRangeFinished(int min, int max, int additional);
    void setIDPopupClosed(SetIDPopup*, int);
    bool getOption(const std::string& option) const;
    int getOptionInt(const std::string& option) const;
    bool toggleOption(const std::string& option);
    void setOption(const std::string& option, bool value);
    void setOptionInt(const std::string& option, int value);
    BISearchObject getSearchObject();
    void setSearchObject(const BISearchObject& searchObj);
    void setFromRangeItem(const std::string& option, const BISearchObject::RangeItem& item);
    void setToRangeItem(BISearchObject::RangeItem& item, const std::string& option) const;
    void keyDown(enumKeyCodes key, double timestamp);

    ~ProfileSearchOptions() {
        log::info("Destroying ProfileSearchOptions");
    }
};