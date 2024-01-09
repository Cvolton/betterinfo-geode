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
    /**
     * Helpers
    */
   void refreshButtonVisibility() {
        if(auto searchMenu = getChildByID("search-menu")) {
            if(auto firstButton = searchMenu->getChildByID("first-button"_spr)) {
                firstButton->setVisible(m_searchObject->m_page > 0);
            }
        }

        if(auto pageMenu = getChildByID("page-menu")) {
            if(auto lastButton = pageMenu->getChildByID("last-button"_spr)) {
                lastButton->setVisible(this->m_itemCount > BetterInfo::levelsPerPage(this->m_searchObject));
            }
            if(auto randomButton = pageMenu->getChildByID("random-button"_spr)) {
                randomButton->setVisible(this->m_itemCount > BetterInfo::levelsPerPage(this->m_searchObject));
            }
        }
   }

    /**
     * Hooks
     */
    void setupPageInfo(gd::string a1, const char* a2) {
        //TODO: utilize geode layout system (needs modifications in loader to make the layout in the first place)
        LevelBrowserLayer::setupPageInfo(a1, a2);

        if(this->m_itemCount == 9999 || BetterInfo::isFalseTotal(this->m_searchObject)) this->m_rightArrow->setVisible(true);

        /*CCMenu* menu = static_cast<CCMenu*>(this->m_rightArrow->getParent());
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        bool isLocal = BetterInfo::isLocal(this->m_searchObject);
        bool isSavedWithPlus = this->m_searchObject->m_searchType == SearchType::SavedLevels || this->m_searchObject->m_searchType == SearchType::FavouriteLevels;

        if(isSavedWithPlus){
            if(BetterInfo::isSavedFiltered() && this->m_countText) this->m_countText->setString((std::string("(Filtered) ") + this->m_countText->getString()).c_str());

        }*/
        refreshButtonVisibility();
        

        

        /*if(this->m_itemCount <= BetterInfo::levelsPerPage(this->m_searchObject)) return;

        */
    }

    bool init(GJSearchObject* searchObj) {
        if(!LevelBrowserLayer::init(searchObj)) return false;

        if(auto pageMenu = getChildByID("page-menu")) {
            /**
             * Random button
            */
            auto randomSprite = BetterInfo::createWithBISpriteFrameName("BI_randomBtn_001.png");
            randomSprite->setScale(0.9f);
            auto randomBtn = CCMenuItemSpriteExtra::create(
                randomSprite,
                this,
                menu_selector(BILevelBrowserLayer::onLevelBrowserRandom)
            );
            randomBtn->setID("random-button"_spr);
            pageMenu->addChild(randomBtn);

            /**
             * Last page button
            */
            if(!BetterInfo::isLocal(this->m_searchObject)){
                auto firstArrow = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
                firstArrow->setPosition({35,25});
                firstArrow->setFlipX(true);
                auto secondArrow = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
                secondArrow->setPosition({15,25});
                secondArrow->setFlipX(true);
                auto arrowParent = CCNode::create();
                arrowParent->setContentSize({50,50});
                arrowParent->addChild(firstArrow);
                arrowParent->addChild(secondArrow);
                arrowParent->setScale(.4f);
                auto lastBtn = CCMenuItemSpriteExtra::create(
                    arrowParent,
                    this,
                    menu_selector(BILevelBrowserLayer::onLevelBrowserLast)
                );
                lastBtn->setID("last-button"_spr);
                pageMenu->addChild(lastBtn);
            }

            /**
             * Page menu done
            */
            pageMenu->updateLayout();
        }

        if(auto searchMenu = getChildByID("search-menu")) {
            /**
             * Filter button
            */
            bool isScreenWithoutFilterBtn = !m_searchObject->isLevelSearchObject() || m_searchObject->m_searchType == SearchType::MyLevels;

            auto filterSprite = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
            filterSprite->setScale(0.7f);
            auto filterButton = CCMenuItemSpriteExtra::create(
                filterSprite,
                this,
                menu_selector(BILevelBrowserLayer::onLevelBrowserFilter)
            );
            if(!isScreenWithoutFilterBtn) searchMenu->addChild(filterButton);
            filterButton->setID("filter-button"_spr);

            /**
             * First button
            */
            auto firstArrow = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
            firstArrow->setPosition({35,34.5});
            auto secondArrow = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
            secondArrow->setPosition({15,34.5});
            auto arrowParent = CCNode::create();
            arrowParent->setContentSize({50,69});
            arrowParent->addChild(firstArrow);
            arrowParent->addChild(secondArrow);
            arrowParent->setScale(.4f);
            auto firstBtn = CCMenuItemSpriteExtra::create(
                arrowParent,
                this,
                menu_selector(BILevelBrowserLayer::onLevelBrowserFirst)
            );
            firstBtn->setID("first-button"_spr);
            searchMenu->addChild(firstBtn);

            /**
             * Search menu done
            */
            searchMenu->updateLayout();
        }

        if(auto infoMenu = getChildByID("info-menu")) {
            /**
             * Star button
            */
            if(!BetterInfo::isStarUseless(this->m_searchObject)) {
                auto starButton = CCMenuItemSpriteExtra::create(
                    CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png"),
                    this,
                    menu_selector(BILevelBrowserLayer::onLevelBrowserStar)
                );
                if(!(this->m_searchObject->m_starFilter)) starButton->setColor({125,125,125});
                starButton->setZOrder(-2); //place above info btn
                starButton->setID("star-button"_spr);
                infoMenu->addChild(starButton);
                infoMenu->updateLayout();
            }
        }

        /**
         * Ending steps
        */
        refreshButtonVisibility();

        

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