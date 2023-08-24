#include "LevelSearchViewLayer.h"
#include "JumpToPageLayer.h"
#include "ProfileSearchOptions.h"
#include "../utils.hpp"

#include <fmt/format.h>

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

    auto GLM = GameLevelManager::sharedState();
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto backgroundSprite = CCSprite::create("game_bg_14_001.png");
    bool controllerConnected = CCApplication::sharedApplication()->getControllerConnected();

    backgroundSprite->setScale(winSize.width / backgroundSprite->getContentSize().width);
    backgroundSprite->setAnchorPoint({0, 0});
    backgroundSprite->setPosition({0,-75});
    backgroundSprite->setColor({164, 0, 255}); //purple
    backgroundSprite->setZOrder(-2);
    addChild(backgroundSprite);

    auto menu = CCMenu::create();
    addChild(menu);
    
    auto backBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
        this,
        menu_selector(LevelSearchViewLayer::onBack)
    );

    backBtn->setPosition({(-winSize.width / 2) + 25, (winSize.height / 2) - 25});
    menu->addChild(backBtn);

    setTouchEnabled(true);
    setKeypadEnabled(true);

    //corners
    auto cornerBL = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    cornerBL->setPosition({0,0});
    cornerBL->setAnchorPoint({0,0});
    addChild(cornerBL, -1);

    auto cornerBR = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    cornerBR->setPosition({winSize.width,0});
    cornerBR->setAnchorPoint({0,0});
    cornerBR->setRotation(270);
    addChild(cornerBR, -1);

    m_statusText = CCLabelBMFont::create("Waiting", "goldFont.fnt");
    m_statusText->setPosition({winSize.width / 2, winSize.height / 2 - 147});
    m_statusText->setScale(.7f);
    addChild(m_statusText);

    auto prevSprite = CCSprite::createWithSpriteFrameName(controllerConnected ? "controllerBtn_DPad_Left_001.png" : "GJ_arrow_03_001.png");
    m_prevBtn = CCMenuItemSpriteExtra::create(
        prevSprite,
        this,
        menu_selector(LevelSearchViewLayer::onPrev)
    );
    m_prevBtn->setPosition({- (winSize.width / 2) + 25, 0});
    menu->addChild(m_prevBtn);

    auto nextSprite = CCSprite::createWithSpriteFrameName(controllerConnected ? "controllerBtn_DPad_Right_001.png" : "GJ_arrow_03_001.png");
    if(!controllerConnected) nextSprite->setFlipX(true);
    m_nextBtn = CCMenuItemSpriteExtra::create(
        nextSprite,
        this,
        menu_selector(LevelSearchViewLayer::onNext)
    );
    m_nextBtn->setPosition({+ (winSize.width / 2) - 25, 0});
    menu->addChild(m_nextBtn);

    m_counter = CCLabelBMFont::create("0 to 0", "goldFont.fnt");
    m_counter->setAnchorPoint({ 1.f, 1.f });
    m_counter->setPosition(winSize - CCPoint(7,3));
    m_counter->setScale(0.5f);
    addChild(m_counter);

    m_searchObj = searchObj;

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

    auto GLM = GameLevelManager::sharedState();
    GLM->m_onlineListDelegate = nullptr;

    if(m_gjSearchObjOptimized) {
        m_gjSearchObjOptimized->release();
        m_gjSearchObjOptimized = nullptr;
    }

    if(m_gjSearchObj) m_gjSearchObj->m_page = 0;

    if(!m_loadedLevels) return;

    for(size_t i = 0; i < m_loadedLevels->count(); i++) {
        auto level = static_cast<GJGameLevel*>(m_loadedLevels->objectAtIndex(i));
        level->release();
    }

    m_loadedLevels->release();
    m_loadedLevels = nullptr;
}

void LevelSearchViewLayer::reload() {
    unload();

    resetUnloadedLevels();

    m_loadedLevels = CCArray::create();
    m_loadedLevels->retain();

    loadPage(true);
    startLoading();
}

void LevelSearchViewLayer::startLoading(){
    if(!m_gjSearchObjOptimized && m_gjSearchObj) optimizeSearchObject();

    if((m_unloadedLevels.empty() && !m_gjSearchObjOptimized) || (m_page + 2) * 10 < m_loadedLevels->count() || (m_gjSearchObjOptimized && m_gjSearchObjOptimized->m_searchType == SearchType::MapPackOnClick && m_gjSearchObjOptimized->m_page > 0)) {
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

        //workaround required to have type 26 working
        toDownload << "&gameVersion=22";

        searchObj = GJSearchObject::create(starFilter ? SearchType::MapPackOnClick : (SearchType)26, toDownload.str());

    } else if(m_gjSearchObjOptimized) {
        searchObj = m_gjSearchObjOptimized;
    }

    if(!searchObj) return;

    auto GLM = GameLevelManager::sharedState();
    auto storedLevels = GLM->getStoredOnlineLevels(searchObj->getKey());
    if(storedLevels) {
        searchObj->m_page += 1;
        loadListFinished(storedLevels, "");
    } else {
        m_gjSearchObjLoaded = searchObj;
        searchObj->retain();
        this->getScheduler()->scheduleSelector(schedule_selector(LevelSearchViewLayer::queueLoad), this, 1, 0, 0.5f, false);
    }
}

void LevelSearchViewLayer::queueLoad(float dt) {
    auto GLM = GameLevelManager::sharedState();
    GLM->m_onlineListDelegate = this;
    GLM->getOnlineLevels(m_gjSearchObjLoaded);
    m_gjSearchObjLoaded->m_page += 1;
    m_gjSearchObjLoaded->release();
}

void LevelSearchViewLayer::loadPage(bool reload){
    if(!m_loadedLevels) return;

    auto currentPage = CCArray::create();
    auto end = (m_page + 1) * 10;
    end = m_loadedLevels->count() < end ? m_loadedLevels->count() : end;
    for(size_t i = m_page * 10; i < end; i++) {
        currentPage->addObject(m_loadedLevels->objectAtIndex(i));
    }

    m_firstIndex = (m_page * 10) + 1;
    m_lastIndex = (m_page * 10) + currentPage->count();
    m_totalAmount = m_unloadedLevels.size() + m_loadedLevels->count();
    m_counter->setCString(fmt::format("{} to {} of {} / {}", m_firstIndex, m_lastIndex, m_loadedLevels->count(), m_totalAmount).c_str());

    if(m_page == 0) m_prevBtn->setVisible(false);
    else m_prevBtn->setVisible(true);

    if(m_loadedLevels->count() > m_lastIndex) m_nextBtn->setVisible(true);
    else m_nextBtn->setVisible(false);

    if(!reload && m_shownLevels == currentPage->count()) return;
    m_shownLevels = currentPage->count();

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    if(m_listLayer != nullptr) m_listLayer->removeFromParentAndCleanup(true);

    m_listView = LevelSearchListView::create(currentPage, 356.f, 220.f);
    m_listLayer = GJListLayer::create(m_listView, "Levels", {191, 114, 62, 255}, 356.f, 220.f);
    m_listLayer->setPosition(winSize / 2 - m_listLayer->getScaledContentSize() / 2 - CCPoint(0,1));
    addChild(m_listLayer);
}

void LevelSearchViewLayer::keyBackClicked() {
    setTouchEnabled(false);
    setKeypadEnabled(false);
    unload();
    if(m_gjSearchObj) m_gjSearchObj->release();
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
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

void LevelSearchViewLayer::loadListFinished(cocos2d::CCArray* levels, const char*) {
    if(!m_loadedLevels) return;

    for(size_t i = 0; i < levels->count(); i++) {
        auto level = static_cast<GJGameLevel*>(levels->objectAtIndex(i));
        if(level == nullptr) continue;

        level->retain();
        if(BetterInfo::levelMatchesObject(level, m_searchObj)) m_loadedLevels->addObject(level);
    }

    loadPage(false);

    startLoading();
}

void LevelSearchViewLayer::loadListFailed(const char*) {
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
        (m_loadedLevels->count() > m_lastIndex ? "Loading (next page)" : "Loading (current page)")
    );
}

void LevelSearchViewLayer::onPrev(cocos2d::CCObject*) {
    if(m_page == 0) return;
    m_page--;
    loadPage(true);
}

void LevelSearchViewLayer::onNext(cocos2d::CCObject*) {
    m_page++;
    loadPage(true);
    startLoading();
}

void LevelSearchViewLayer::onFilters(cocos2d::CCObject*) {
    auto searchOptions = ProfileSearchOptions::create(nullptr, "", this);
    searchOptions->setSearchObject(m_searchObj);
    searchOptions->show();
}

void LevelSearchViewLayer::keyDown(enumKeyCodes key){
    switch(key){
        case KEY_Left:
        case CONTROLLER_Left:
            if(m_prevBtn->isVisible() == true) onPrev(nullptr);
            break;
        case KEY_Right:
        case CONTROLLER_Right:
            if(m_nextBtn->isVisible() == true) onNext(nullptr);
            break;
        default:
            CCLayer::keyDown(key);
    }
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
        "As a result it may take a while to load all results depending on the filter combinations.",

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
            !m_searchObj.idRange.enabled
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