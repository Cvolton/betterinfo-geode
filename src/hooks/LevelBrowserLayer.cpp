#include <Geode/Bindings.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>

#include "../utils.hpp"
#include "../layers/LevelBrowserEndLayer.h"
#include "../layers/LevelFiltering/LevelSearchViewLayer.h"
#include "../layers/LevelFiltering/ProfileSearchOptions.h"

using namespace geode::prelude;

class $modify(BILevelBrowserLayer, LevelBrowserLayer) {
    /*
     * Callbacks
     */
    void onLevelBrowserFilter(CCObject* sender){
        if(BetterInfo::isLocal(this->m_searchObject)) ProfileSearchOptions::create(this, "user_search")->show();
        else {
            auto layer = LevelSearchViewLayer::scene(this->m_searchObject);
            auto transitionFade = CCTransitionFade::create(0.5, layer);
            CCDirector::sharedDirector()->pushScene(transitionFade);
        }
    }    

    void onLevelBrowserRandom(CCObject* sender){
        if(this->m_searchObject == nullptr) return;
        int pageMax = this->m_itemCount / BetterInfo::levelsPerPage(this->m_searchObject);

        int pageToLoad = BetterInfo::randomNumber(0, pageMax);

        this->m_searchObject->m_page = pageToLoad;
        this->loadPage(this->m_searchObject);
    }

    void onLevelBrowserFirst(CCObject* sender){
        this->m_searchObject->m_page = 0;
        this->loadPage(this->m_searchObject);
    }

    void onLevelBrowserLast(CCObject* sender){
        if(this->m_itemCount == 9999 || BetterInfo::isFalseTotal(this->m_searchObject)) {
            LevelBrowserEndLayer::create(this)->show();
            return;
        }

        this->m_searchObject->m_page = (this->m_itemCount - 1) / BetterInfo::levelsPerPage(this->m_searchObject);
        this->loadPage(this->m_searchObject);
    }

    void onLevelBrowserStar(CCObject* sender){
        if(this->m_searchObject == nullptr) return;

        this->m_searchObject->m_starFilter = !(this->m_searchObject->m_starFilter);
        this->loadPage(this->m_searchObject);

        auto button = static_cast<CCMenuItemSpriteExtra*>(sender);
        if(this->m_searchObject->m_starFilter) button->setColor({255, 255, 255});
        else button->setColor({125,125,125});
    }

    /*
     * Helpers
     */
    CCMenuItemSpriteExtra* createRandomBtn() {
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        bool isLocal = BetterInfo::isLocal(this->m_searchObject);

        auto randomSprite = BetterInfo::createWithBISpriteFrameName("BI_randomBtn_001.png");
        randomSprite->setScale(0.9f);
        auto randomBtn = CCMenuItemSpriteExtra::create(
            randomSprite,
            this,
            menu_selector(BILevelBrowserLayer::onLevelBrowserRandom)
        );
        randomBtn->setPosition({ (winSize.width / 2) - 23, 88});
        randomBtn->setID("bi-random-button");
        if(isLocal) randomBtn->setPosition({(winSize.width / 2) - 58, 122});
        return randomBtn;
    }

    /*
     * Hooks
     */
    void setupPageInfo(gd::string a1, const char* a2) {
        //TODO: utilize geode layout system (needs modifications in loader to make the layout in the first place)
        LevelBrowserLayer::setupPageInfo(a1, a2);

        if(this->m_itemCount == 9999 || BetterInfo::isFalseTotal(this->m_searchObject)) this->m_rightArrow->setVisible(true);

        CCMenu* menu = static_cast<CCMenu*>(this->m_rightArrow->getParent());
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        bool isLocal = BetterInfo::isLocal(this->m_searchObject);
        bool isSavedWithPlus = this->m_searchObject->m_searchType == SearchType::SavedLevels || this->m_searchObject->m_searchType == SearchType::FavouriteLevels;

        if(isSavedWithPlus){
            if(BetterInfo::isSavedFiltered() && this->m_countText) this->m_countText->setString((std::string("(Filtered) ") + this->m_countText->getString()).c_str());

        }

        bool isScreenWithoutFilterBtn = this->m_searchObject->m_searchType == SearchType::Users || this->m_searchObject->m_searchType == SearchType::MapPack || this->m_searchObject->m_searchType == SearchType::MyLevels;
        if(menu->getChildByID("bi-filter-button") == nullptr && !isScreenWithoutFilterBtn) {
            auto filterSprite = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
            filterSprite->setScale(0.7f);
            auto filterButton = CCMenuItemSpriteExtra::create(
                filterSprite,
                this,
                menu_selector(BILevelBrowserLayer::onLevelBrowserFilter)
            );
            menu->addChild(filterButton);
            filterButton->setPosition({- (winSize.width / 2) + (isSavedWithPlus ? 64 : 26), 92});
            filterButton->setSizeMult(1.2f);
            filterButton->setID("bi-filter-button");
        }

        if(menu->getChildByID("bi-star-button")) return;

        if(!BetterInfo::isStarUseless(this->m_searchObject)) {
            auto starButton = CCMenuItemSpriteExtra::create(
                CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png"),
                this,
                menu_selector(BILevelBrowserLayer::onLevelBrowserStar)
            );
            menu->addChild(starButton);
            starButton->setPosition({- (winSize.width / 2) + 30, - (winSize.height / 2) + 58});
            if(!(this->m_searchObject->m_starFilter)) starButton->setColor({125,125,125});
            starButton->setSizeMult(1.2f);
            starButton->setID("bi-star-button");
        }

        CCMenu* searchMenu = static_cast<CCMenu*>(this->getChildByID("search-menu"));
        auto firstBtn = static_cast<CCNode*>(menu->getChildByID("bi-first-button"));
        if(!firstBtn && searchMenu) firstBtn = static_cast<CCNode*>(searchMenu->getChildByID("bi-first-button"));

        if(firstBtn) {
            if(this->m_searchObject->m_page == 0) firstBtn->setVisible(false);
            else firstBtn->setVisible(true);
            return;
        }

        auto doubleArrowLeft = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
        auto arrowLeft = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
        arrowLeft->setPosition({0,20});
        doubleArrowLeft->addChild(arrowLeft);
        doubleArrowLeft->setScale(0.5f);
        firstBtn = CCMenuItemSpriteExtra::create(
            doubleArrowLeft,
            this,
            menu_selector(BILevelBrowserLayer::onLevelBrowserFirst)
        );
        firstBtn->setID("bi-first-button");
        //259 60
        firstBtn->setPosition({ - (winSize.width / 2) + 26, 60});
        if(isLocal) firstBtn->setPosition({ - (winSize.width / 2) + 67, 60});
        if(this->m_searchObject->m_page > 0) {
            if(!searchMenu) menu->addChild(firstBtn);
            else {
                searchMenu->addChild(firstBtn);
                searchMenu->updateLayout();
            }
        }

        if(this->m_itemCount <= BetterInfo::levelsPerPage(this->m_searchObject)) return;

        CCMenu* pageMenu = static_cast<CCMenu*>(this->getChildByID("page-menu"));
        if(menu->getChildByID("bi-random-button") || (pageMenu && pageMenu->getChildByID("bi-random-button"))) return;

        auto randomBtn = createRandomBtn();
        if(m_searchObject->m_searchType != SearchType::MyLevels) menu->addChild(randomBtn);

        if(!isLocal){
            auto doubleArrow = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
            auto arrow = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
            arrow->setPosition({31.5,20});
            doubleArrow->addChild(arrow);
            doubleArrow->setScale(0.5f);
            doubleArrow->setFlipX(true);
            arrow->setFlipX(true);
            auto lastBtn = CCMenuItemSpriteExtra::create(
                doubleArrow,
                this,
                menu_selector(BILevelBrowserLayer::onLevelBrowserLast)
            );
            //259 60
            lastBtn->setPosition({ (winSize.width / 2) - 26, 60});
            lastBtn->setID("bi-last-button");
            menu->addChild(lastBtn);
        }
    }

    bool init(GJSearchObject* searchObj) {
        if(!LevelBrowserLayer::init(searchObj)) return false;

        if(m_searchObject->m_searchType == SearchType::MyLevels) {
            CCMenu* pageMenu = static_cast<CCMenu*>(this->getChildByID("page-menu"));
            auto randomBtn = createRandomBtn();
            pageMenu->addChild(randomBtn);
            pageMenu->updateLayout();
        }

        return true;
    }

    void onInfo(CCObject* sender) {
        FLAlertLayer::create(nullptr, "Info", 
            "<cg>Green</c> users are unregistered.\n"
            "<cy>Gold</c> users are unique registered.\n"
            "<cl>Blue</c> songs are official.\n"
            "<cp>Purple</c> songs are custom.\n"
            "<co>Orange</c> songs are custom, not from Newgrounds.\n",
        "OK", nullptr, 360)->show();
    }
};