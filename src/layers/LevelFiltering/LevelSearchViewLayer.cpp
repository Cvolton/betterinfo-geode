#include "LevelSearchViewLayer.h"
#include "Geode/loader/Log.hpp"
#include "ProfileSearchOptions.h"
#include "../../utils.hpp"

#include <fmt/format.h>

static inline std::unordered_map<std::string, cocos2d::CCArray*> s_cache;

LevelSearchViewLayer* LevelSearchViewLayer::create(std::deque<GJGameLevel*> allLevels, BISearchObject searchObj) {
    auto ret = new LevelSearchViewLayer();
    if (ret && ret->init(allLevels, searchObj)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

LevelSearchViewLayer* LevelSearchViewLayer::create(GJSearchObject* gjSearchObj, BISearchObject searchObj) {
    auto ret = new LevelSearchViewLayer();
    if (ret && ret->init(gjSearchObj, searchObj)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool LevelSearchViewLayer::init(std::deque<GJGameLevel*> allLevels, BISearchObject searchObj) {
    m_allLevels = allLevels;
    return init(searchObj);
}

bool LevelSearchViewLayer::init(GJSearchObject* gjSearchObj, BISearchObject searchObj) {
    m_gjSearchObj = gjSearchObj;
    m_gjSearchObj->retain();
    return init(searchObj);
}

bool LevelSearchViewLayer::init(BISearchObject searchObj) {
    BIViewLayer::init();

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    m_statusText = CCLabelBMFont::create("Waiting", "goldFont.fnt");
    m_statusText->setPosition({winSize.width / 2, winSize.height / 2 - 147});
    m_statusText->setScale(.7f);
    addChild(m_statusText);

    m_searchObj = searchObj;

    auto menu = CCMenu::create();

    auto buttonSprite = ButtonSprite::create("Filters", (int)(90*0.5), true, "bigFont.fnt", "GJ_button_01.png", 44*0.5f, 0.5f);
    auto buttonButton = CCMenuItemSpriteExtra::create(
        buttonSprite,
        this,
        menu_selector(LevelSearchViewLayer::onFilters)
    );
    buttonButton->setSizeMult(1.2f);
    buttonButton->setPosition({- winSize.width / 2, - winSize.height / 2});
    buttonButton->setAnchorPoint({0,0});
    menu->addChild(buttonButton);

    auto infoSprite = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    auto infoBtn = CCMenuItemSpriteExtra::create(
        infoSprite,
        this,
        menu_selector(LevelSearchViewLayer::onInfo)
    );
    infoBtn->setPosition({+ (winSize.width / 2) - 25, - (winSize.height / 2) + 25});
    menu->addChild(infoBtn);

    addChild(menu);

    reload();

    if(m_gjSearchObj && !(Mod::get()->getSavedValue<bool>("lsvl_seen_info"))) showInfoDialog();

    return true;
}

void LevelSearchViewLayer::unload() {
    m_page = 0;
    m_shownLevels = 0;
    m_firstIndex = 0;
    m_lastIndex = 0;
    m_totalAmount = 0;

    if(m_gjSearchObjOptimized) {
        m_gjSearchObjOptimized->release();
        m_gjSearchObjOptimized = nullptr;
    }

    if(m_gjSearchObj) m_gjSearchObj->m_page = 0;

    if(!m_data) return;

    m_data->release();
    m_data = nullptr;
}

void LevelSearchViewLayer::reload() {
    unload();

    resetUnloadedLevels();

    m_data = CCArray::create();
    m_data->retain();

    loadPage(true);
    startLoading();
}

void LevelSearchViewLayer::startLoading(){
    if(!m_data) return;

    if(!m_gjSearchObjOptimized && m_gjSearchObj) optimizeSearchObject();

    if((m_unloadedLevels.empty() && !m_gjSearchObjOptimized) || ( !m_allLocal && ( (m_page + 2) * 10 < m_data->count() ) ) || (m_gjSearchObjOptimized && m_gjSearchObjOptimized->m_searchType == SearchType::MapPackOnClick && m_gjSearchObjOptimized->m_page > 0)) {
        setTextStatus(true);
        return;
    }

    setTextStatus(false);
    GJSearchObject* searchObj = nullptr;

    if(!m_unloadedLevels.empty()) {
        bool starFilter = m_searchObj.star || m_searchObj.starRange.min > 0;
        size_t levelsPerRequest = (starFilter) ? 300 : 100;

        std::stringstream toDownload;
        bool first = true;
        for(size_t i = 0; i < ((m_unloadedLevels.size() < levelsPerRequest) ? m_unloadedLevels.size() : levelsPerRequest); i++) {
            if(!first) toDownload << ",";
            toDownload << m_unloadedLevels[0]->m_levelID.value();
            m_unloadedLevels.pop_front();
            first = false;
        }

        searchObj = GJSearchObject::create(starFilter ? SearchType::MapPackOnClick : (SearchType)26, toDownload.str());

    } else if(m_gjSearchObjOptimized) {
        searchObj = m_gjSearchObjOptimized;
    }

    if(!searchObj) return;

    auto GLM = GameLevelManager::sharedState();
    if(auto&& key = s_cache.find(searchObj->getKey()); key != s_cache.end()) {
        searchObj->m_page += 1;
        loadLevelsFinished(key->second, "");
    } else if(auto storedLevels = GLM->getStoredOnlineLevels(searchObj->getKey())) {
        searchObj->m_page += 1;
        loadLevelsFinished(storedLevels, "");
    } else {
        m_gjSearchObjLoaded = searchObj;
        searchObj->retain();
        // this amounts to 80 requests per second, which is 20 below the server limit
        auto time = 0.75 - (TimeUtils::getFullDoubleTime() - m_lastLoadTime);
        log::debug("Time: {}", time);
        if(time < 0) {
            this->queueLoad(0);
        } else {
            this->getScheduler()->scheduleSelector(schedule_selector(LevelSearchViewLayer::queueLoad), this, 1, 0, time, false);
        }
        m_lastLoadTime = TimeUtils::getFullDoubleTime();
    }
}

void LevelSearchViewLayer::queueLoad(float dt) {
    auto GLM = GameLevelManager::sharedState();
    GLM->m_levelManagerDelegate = this;
    GLM->getOnlineLevels(m_gjSearchObjLoaded);
    m_gjSearchObjLoaded->m_page += 1;
    m_gjSearchObjLoaded->release();
    m_allLocal = false;
}

void LevelSearchViewLayer::loadPage(bool reload){
    if(!m_data) return;

    auto currentPage = trimData();

    m_firstIndex = (m_page * 10) + 1;
    m_lastIndex = (m_page * 10) + currentPage->count();
    m_totalAmount = m_unloadedLevels.size() + m_data->count();

    updateCounter();

    if(!reload && m_shownLevels == currentPage->count()) return;
    m_shownLevels = currentPage->count();

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    m_listView = CustomListView::create(currentPage, BoomListType::Level, 220.f, 356.f);
    BIViewLayer::loadPage();
}

void LevelSearchViewLayer::updateCounter() {
    if(!m_data) return;

    BIViewLayer::updateCounter();
    
    m_counter->setVisible(true);
    m_counter->setCString(fmt::format("{} to {} of {} / {}", m_firstIndex, m_lastIndex, m_data->count(), m_totalAmount).c_str());
}

void LevelSearchViewLayer::loadPage() {
    loadPage(true);
    startLoading();
}

void LevelSearchViewLayer::keyBackClicked() {
    unload();
    if(m_gjSearchObj) m_gjSearchObj->release();

    BIViewLayer::keyBackClicked();
}

void LevelSearchViewLayer::onBack(CCObject* object) {
    keyBackClicked();
}

CCScene* LevelSearchViewLayer::scene(std::deque<GJGameLevel*> allLevels, BISearchObject searchObj) {
    auto layer = LevelSearchViewLayer::create(allLevels, searchObj);
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}

CCScene* LevelSearchViewLayer::scene(GJSearchObject* gjSearchObj, BISearchObject searchObj) {
    auto layer = LevelSearchViewLayer::create(gjSearchObj, searchObj);
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}

void LevelSearchViewLayer::loadLevelsFinished(cocos2d::CCArray* levels, const char* key) {
    if(!m_data) return;

    if(!std::string_view(key).empty()) {
        s_cache[key] = levels;
        levels->retain();
    }

    for(size_t i = 0; i < levels->count(); i++) {
        auto level = static_cast<GJGameLevel*>(levels->objectAtIndex(i));
        if(level == nullptr) continue;

        if(BetterInfo::levelMatchesObject(level, m_searchObj)) m_data->addObject(level);
    }

    loadPage(false);

    startLoading();
}

void LevelSearchViewLayer::loadLevelsFailed(const char*) {
    if(!m_gjSearchObj) startLoading();
    else {
        setTextStatus(true);
        if(m_gjSearchObjOptimized) m_gjSearchObjOptimized->m_page -= 1;
    }
}

void LevelSearchViewLayer::setupPageInfo(gd::string, const char*) {

}

void LevelSearchViewLayer::setTextStatus(bool finished) {
    if(m_statusText) m_statusText->setString(
        finished ? "Finished" : 
        m_gjSearchObjOptimized ? fmt::format("Loading (online page {})", m_gjSearchObjOptimized->m_page).c_str() :
        (m_data->count() > m_lastIndex ? "Loading (next page)" : "Loading (current page)")
    );
}

void LevelSearchViewLayer::onFilters(cocos2d::CCObject*) {
    auto searchOptions = ProfileSearchOptions::create(nullptr, "", this);
    searchOptions->setSearchObject(m_searchObj);
    searchOptions->show();
}

void LevelSearchViewLayer::onInfo(cocos2d::CCObject*) {
    LevelSearchViewLayer::showInfoDialog();
}

void LevelSearchViewLayer::onSearchObjectFinished(const BISearchObject& searchObj) {
    m_searchObj = searchObj;
    reload();
}

void LevelSearchViewLayer::showInfoDialog() {
    auto alert = FLAlertLayer::create(
        nullptr, 
        "Filtered Level Search", 

        "This allows you to further filter search results in ways the server usually wouldn't allow you.\n"
        "\n"
        "All of the filtering is done <cg>client-side</c>.\n"
        "As a result it may take a while to load all results depending on the filter combinations.\n"
        "\n"
        "Press the <cy>Filters</c> <cp>button</c> in the bottom left corner to change the enabled search filters.",

        "OK", 
        nullptr,
        400
    );
    alert->m_scene = this;
    alert->show();

    Mod::get()->setSavedValue<bool>("lsvl_seen_info", true);
}

void LevelSearchViewLayer::optimizeSearchObject() {
    if(!m_gjSearchObj) return;
    if(m_gjSearchObjOptimized) m_gjSearchObjOptimized->release();

    /*
        SearchType m_nScreenID; //236 android
        std::string m_sSearchQuery;
        std::string m_sDifficulty;
        std::string m_sLength;
        int m_nPage;
        bool m_bNoStarFilter;
        int m_nTotal;
        bool m_bUncompletedFilter; //264 android
        bool m_bCompletedFilter;
        int m_eDemonFilter;
        int currentFolder; // might be unsigned, but then again its robtop
        int m_nSongID;
        bool m_bCustomSongFilter;
        bool m_bSongFilter;
    */

    m_gjSearchObjOptimized = GJSearchObject::createFromKey(m_gjSearchObj->getKey());
    m_gjSearchObjOptimized->retain();

    //TODO: if server optimization disabled: return

    if(m_searchObj.star || m_searchObj.starRange.enabled && m_searchObj.starRange.min > 0) m_gjSearchObjOptimized->m_starFilter = true;

    if(!BetterInfo::isAdvancedEnabled(m_gjSearchObjOptimized)) return;

    //TODO: if star min > 0, filter diff based on star values
    if(m_searchObj.featured && m_gjSearchObjOptimized->m_starFilter == true) m_gjSearchObjOptimized->m_featuredFilter = true;
    if(m_searchObj.original) m_gjSearchObjOptimized->m_originalFilter = true;
    if(m_searchObj.twoPlayer) m_gjSearchObjOptimized->m_twoPlayerFilter = true;
    if(m_searchObj.coins) m_gjSearchObjOptimized->m_coinsFilter = true;
    if(m_searchObj.epic) m_gjSearchObjOptimized->m_epicFilter = true;
    if(m_searchObj.noStar) m_gjSearchObjOptimized->m_noStarFilter = true;
    //TODO: song filters
    //TODO: if filters DEMON && DEMON DIFF only!! - filter demon diff server-side
    //TODO: filter diff based on diff values IF auto && demon NOT SET
    //TODO: filter length based on length values
    //TODO: completed

}

void LevelSearchViewLayer::resetUnloadedLevels() {
    if(m_allLevels.empty() || 
        (
            !m_searchObj.completed && !m_searchObj.completedOrbs && !m_searchObj.completedLeaderboard &&
            !m_searchObj.uncompleted && !m_searchObj.uncompletedOrbs && !m_searchObj.uncompletedLeaderboard &&
            !m_searchObj.percentage.enabled && !m_searchObj.percentageOrbs.enabled && !m_searchObj.percentageLeaderboard.enabled &&
            !m_searchObj.idRange.enabled && !m_searchObj.favorite
        )
    ) {
        m_unloadedLevels = m_allLevels;
    } else {
        m_unloadedLevels.clear();
        for(const auto& level : m_allLevels) {
            if(!BetterInfo::levelProgressMatchesObject(level, m_searchObj)) continue;

            m_unloadedLevels.push_back(level);
        }
    }
}

int LevelSearchViewLayer::resultsPerPage() const {
    return 10;
}

void LevelSearchViewLayer::onEnter() {
    BIViewLayer::onEnter();

    auto GLM = GameLevelManager::sharedState();
    GLM->m_levelManagerDelegate = this;
    
    loadPage();
}

LevelSearchViewLayer::~LevelSearchViewLayer() {
    auto GLM = GameLevelManager::sharedState();
    if(GLM->m_levelManagerDelegate == this) GLM->m_levelManagerDelegate = nullptr;
}

void LevelSearchViewLayer::resetCache() {
    s_cache.clear();
}