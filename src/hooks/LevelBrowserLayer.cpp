#include <Geode/Geode.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/modify/CCLayer.hpp>

#include "../utils.hpp"
#include "../ui/DoubleArrow.h"
#include "../ui/FolderButton.h"
#include "../layers/LevelBrowserEndLayer.h"
#include "../layers/LevelFiltering/LevelSearchViewLayer.h"
#include "../layers/LevelFiltering/ProfileSearchOptions.h"

#include "../managers/BetterInfoCache.h"

using namespace geode::prelude;

static const std::unordered_map<SearchType, const char*> s_labels = {
    { SearchType::HallOfFame, "hallOfFameLabel_001.png"_spr },
};

static const std::unordered_map<SearchType, std::pair<const char*, const char*>> s_infoTexts = {
    // 2.208: remove
    { SearchType::FeaturedLite, {"Weekly Levels", "New <cg>levels</c> are released every <cy>week</c>.\n<cl>Check back later for more updates!</c>" }},
    { SearchType::Bonus, {"Bonus Levels", "New <cg>levels</c> are released from time to time.\n<cl>Check back later for more updates!</c>" }}
};

class BI_DLL 
$modify(BILevelBrowserLayer, LevelBrowserLayer) {
    struct Fields {
        CCMenuItemSpriteExtra* m_biLastPageBtn = nullptr;
        DoubleArrow* m_biLastPageBtnArrow = nullptr;
    };

    static void onModify(auto& self) {
        (void) self.setHookPriority("LevelBrowserLayer::onGoToPage", Priority::VeryLatePre);
    }

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
        int pageMax = (this->m_itemCount - 1) / BetterInfo::levelsPerPage(this->m_searchObject);

        int pageToLoad = BetterInfo::randomNumber(0, pageMax);

        this->m_searchObject->m_page = pageToLoad;
        this->loadPage(this->m_searchObject);
    }

    void onLevelBrowserFirst(CCObject* sender){
        this->m_searchObject->m_page = 0;
        this->loadPage(this->m_searchObject);
    }

    void onLevelBrowserLast(CCObject* sender){
        if(shouldSearchForLastPage()) {
            LevelBrowserEndLayer::create(this, nullptr)->show();
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

    void onLevelBrowserHide(CCObject* sender){
        if(m_searchObject == nullptr || (m_searchObject->m_searchType != SearchType::UsersLevels && !std::string_view(m_searchObject->m_searchQuery).starts_with("&type=6"))) return;

        bool isCurrentlyFiltered = std::string_view(m_searchObject->m_searchQuery).starts_with("&type=");

        if(!isCurrentlyFiltered) {
            m_searchObject->m_searchQuery = fmt::format("&type={}&str={}", (int) m_searchObject->m_searchType, m_searchObject->m_searchQuery);
        } else {
            m_searchObject->m_searchType = SearchType::UsersLevels;
            auto pos = std::string_view(m_searchObject->m_searchQuery).find('=');
            if(pos != std::string_view::npos) {
                pos = std::string_view(m_searchObject->m_searchQuery).find('=', pos + 1);
                if(pos != std::string_view::npos) {
                    m_searchObject->m_searchQuery = std::string(std::string_view(m_searchObject->m_searchQuery).substr(pos + 1));
                }
            }
        }
        this->loadPage(m_searchObject);

        auto button = static_cast<CCMenuItemSpriteExtra*>(sender);
        if(isCurrentlyFiltered) button->setColor({255, 255, 255});
        else button->setColor({125,125,125});

        Notification::create(fmt::format("Unlisted levels {}", isCurrentlyFiltered ? "shown" : "hidden"), CCSprite::createWithSpriteFrameName("hideBtn_001.png"))->show();
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
                lastButton->setVisible(m_rightArrow->isVisible());
            }
            if(auto randomButton = pageMenu->getChildByID("random-button"_spr)) {
                randomButton->setVisible(this->m_itemCount > BetterInfo::levelsPerPage(this->m_searchObject));
            }
        }

        if(auto lastBtn = m_fields->m_biLastPageBtnArrow) {
            lastBtn->usePopupTexture(shouldSearchForLastPage());
        }
    }

    bool canBeLocalFiltered() {
        return BetterInfo::isLocal(m_searchObject) && m_searchObject->isLevelSearchObject() && m_searchObject->m_searchType != SearchType::MyLevels && m_searchObject->m_searchMode == 0;
    }

    void showFilteredText() {
        if(canBeLocalFiltered() && BetterInfo::isSavedFiltered() && this->m_countText) this->m_countText->setString((std::string("(Filtered) ") + this->m_countText->getString()).c_str());
    }

    bool shouldSearchForLastPage() {
        return this->m_itemCount == 9999 || BetterInfo::isFalseTotal(this->m_searchObject);
    }

    /**
     * Hooks
     */
    void onGoToPage(CCObject* sender) {
        static_assert(&LevelBrowserLayer::onGoToPage, "Hook not implemented");

        auto popup = SetIDPopup::create(m_searchObject->m_page + 1, 1, 999999, "Go to Page", "Go", true, 1, 60.f, false, false);
        popup->m_delegate = this;
        popup->show();
    }

    void setupPageInfo(gd::string a1, const char* a2) {
        LevelBrowserLayer::setupPageInfo(a1, a2);

        if(this->m_itemCount == 9999 || BetterInfo::isFalseTotal(this->m_searchObject)) this->m_rightArrow->setVisible(true);

        refreshButtonVisibility();
        showFilteredText();
    }
    
    void loadLevelsFinished(cocos2d::CCArray* levels, char const* key, int type) {
        LevelBrowserLayer::loadLevelsFinished(levels, key, type);

        if(m_searchObject->m_searchMode == 1 && m_searchObject->m_searchType == SearchType::Featured && m_searchObject->m_page == 0) BetterInfoCache::sharedState()->tryShowClaimableListsPopup(this);
    }

    void loadPage(GJSearchObject* searchObj) {
        LevelBrowserLayer::loadPage(searchObj);

        refreshButtonVisibility();
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
             * Folder button
            */
            if(!searchObj->isLevelSearchObject()) {
                if(searchObj->m_searchType == SearchType::MyLists || searchObj->m_searchType == SearchType::FavouriteLists) {
                    /*m_folderBtn = CCMenuItemSpriteExtra::create(
                        CCSprite::createWithSpriteFrameName("gj_folderBtn_001.png"), 
                        nullptr, 
                        this,
                        menu_selector(LevelBrowserLayer::onGoToFolder)
                    );

                    m_folderText = CCLabelBMFont::create("0", "bigFont.fnt");
                    m_folderText->setPosition(m_folderBtn->getNormalImage()->getContentSize() / 2);
                    m_folderText->setScale(.55f);
                    m_folderBtn->getNormalImage()->addChild(m_folderText);

                    pageMenu->insertBefore(m_folderBtn, pageMenu->getChildByID("last-page-button"));*/

                    auto folderBtn = FolderButton::create([this](int id) {
                        m_searchObject->m_folder = id;
                        LevelBrowserLayer::loadPage(m_searchObject);
                    });
                    folderBtn->setDisplayFolder(m_searchObject->m_folder);
                    folderBtn->setIsCreated(m_searchObject->m_searchType == SearchType::MyLists);
                    folderBtn->setPopupLabel("Go to Folder");
                    folderBtn->setID("folder-button"_spr);

                    pageMenu->insertBefore(folderBtn, pageMenu->getChildByID("last-page-button"));
                }
            }

            /**
             * Last page button
            */
            if(!BetterInfo::isLocal(this->m_searchObject)){
                m_fields->m_biLastPageBtn = CCMenuItemSpriteExtra::create(
                    m_fields->m_biLastPageBtnArrow = DoubleArrow::create(true, shouldSearchForLastPage()),
                    this,
                    menu_selector(BILevelBrowserLayer::onLevelBrowserLast)
                );
                m_fields->m_biLastPageBtn->setID("last-button"_spr);
                pageMenu->addChild(m_fields->m_biLastPageBtn);
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
            bool isScreenWithoutFilterBtn = !BetterInfo::isLevelSearchObject(m_searchObject) || m_searchObject->m_searchType == SearchType::MyLevels;

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
            auto firstBtn = CCMenuItemSpriteExtra::create(
                DoubleArrow::create(false),
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

            if(m_searchObject->m_searchType == SearchType::UsersLevels) {
                auto myId = GameManager::sharedState()->m_playerUserID.value();

                bool isMyLevelUnfiltered = m_searchObject->m_searchQuery == fmt::format("{}", myId);
                bool isMyLevelFiltered = m_searchObject->m_searchQuery == fmt::format("{}&", myId);
                if(isMyLevelUnfiltered || isMyLevelFiltered) {
                    auto hideBtn = CCMenuItemSpriteExtra::create(
                        CCSprite::createWithSpriteFrameName("hideBtn_001.png"),
                        this,
                        menu_selector(BILevelBrowserLayer::onLevelBrowserHide)
                    );
                    hideBtn->setID("unlisted-button"_spr);
                    hideBtn->setZOrder(-3);
                    if(isMyLevelFiltered) hideBtn->setColor({125,125,125});
                    infoMenu->addChild(hideBtn);
                    infoMenu->updateLayout();
                }
            }
        }

        if(m_searchObject->m_searchMode == 0 && s_labels.contains(m_searchObject->m_searchType)) {
            auto winSize = CCDirector::sharedDirector()->getWinSize();
            auto label = CCSprite::create(s_labels.at(m_searchObject->m_searchType));
            label->setPosition({(winSize.width / 2), (winSize.height / 2) + 24 + 110});
            label->setID("header-sprite"); //substitutes a vanilla feature, therefore vanilla style ID
            this->addChild(label, 2);
        }

        /**
         * Ending steps
        */
        refreshButtonVisibility();

        return true;
    }

    gd::string getSearchTitle() {
        if(m_searchObject->m_folder > 0 && (m_searchObject->m_searchType == SearchType::MyLists || m_searchObject->m_searchType == SearchType::FavouriteLists)) {
            auto folderName = GameLevelManager::sharedState()->getFolderName(m_searchObject->m_folder, m_searchObject->m_searchType == SearchType::MyLists);

            auto mainTitle = m_searchObject->m_searchType == SearchType::MyLists ? "My Lists" : "Favorite Lists";

            if(folderName.empty()) return fmt::format("{} ({})", mainTitle, m_searchObject->m_folder);

            return fmt::format("{} ({}: {})", mainTitle, m_searchObject->m_folder, folderName);
        }

        if(s_labels.contains(m_searchObject->m_searchType)) return "";

        return LevelBrowserLayer::getSearchTitle();
    }

    void onInfo(CCObject* sender) {
        if(s_infoTexts.contains(m_searchObject->m_searchType)) {
            auto info = s_infoTexts.at(m_searchObject->m_searchType);
            return FLAlertLayer::create(
                nullptr,
                info.first,
                info.second,
                "OK",
                nullptr,
                380.f
            )->show();
        }

        return LevelBrowserLayer::onInfo(sender);
    }
};

class $modify(CCLayer) {
    void onEnterTransitionDidFinish() {
        CCLayer::onEnterTransitionDidFinish();

        if(typeinfo_cast<LevelBrowserLayer*>(this)) reinterpret_cast<BILevelBrowserLayer*>(this)->refreshButtonVisibility();
    }
};