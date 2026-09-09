#include "LeaderboardViewLayer.h"
#include "../managers/BetterInfoOnline.h"
#include "../utils.hpp"

LeaderboardViewLayer* LeaderboardViewLayer::create(BILeaderboardMode mode, int accountID) {
    auto ret = new LeaderboardViewLayer();
    if (ret && ret->init(mode, accountID)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool LeaderboardViewLayer::init(BILeaderboardMode mode, int accountID) {
    BIViewLayer::init(false);

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    m_mode = mode;
    m_accountID = accountID;
    m_title = "Global Leaderboards";
    m_hideTitle = mode == BILeaderboardMode::Top1000;

    //refresh btn
    auto refreshBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png"),
        this,
        menu_selector(LeaderboardViewLayer::onRefresh)
    );
    refreshBtn->setID("refresh-button"_spr);

    auto menuRefresh = CCMenu::create();
    menuRefresh->addChild(refreshBtn);
    menuRefresh->setPosition({winSize.width - 26.75f, 26.75f});
    menuRefresh->setZOrder(2);
    menuRefresh->setID("refresh-menu"_spr);

    this->addChild(menuRefresh);

    setData(CCArray::create());

    loadPage();

    m_rightMenu = CCMenu::create();
    m_rightMenu->setID("right-side-menu"_spr);
    m_rightMenu->setContentSize({ 24.f, 150.f });
    m_rightMenu->setPosition({ winSize.width / 2 + 220.f, winSize.height / 2 });
    m_rightMenu->setLayout(
        ColumnLayout::create()
            ->setGap(11.f)
            ->setAxisAlignment(AxisAlignment::Center)
            ->setAxisReverse(true)
            ->setAutoScale(false)
    );

    m_topMenu = CCMenu::create();
    m_topMenu->setID("top-menu"_spr);
    m_topMenu->setPosition({ winSize.width / 2, winSize.height / 2 + 134.f });
    m_topMenu->setContentSize({ 380.f, 50.f });
    m_topMenu->setLayout(
        RowLayout::create()
            ->setGap(6.f)
            ->setGrowCrossAxis(true)
            ->setCrossAxisOverflow(false)
    );

    this->setupStatBtns();
    this->addChild(m_rightMenu);
    this->addChild(m_topMenu);

    this->loadStat(0, false);

    return true;
}

void LeaderboardViewLayer::onTabChanged(CCObject* object) {
    auto btn = static_cast<TabButton*>(object);
    if(!btn) return;

    m_tab = (BILeaderboardTab) btn->getTag();
    this->loadStat(m_stat, false);

    static bool friendsNoticeShown = false;
    if(!friendsNoticeShown && m_tab == BILeaderboardTab::Friends) {
        Notification::create("Extended friends leaderboard does not\nsupport Diamonds, Coins and User Coins", NotificationIcon::Info, NOTIFICATION_DEFAULT_TIME * 2.f)->show();
        friendsNoticeShown = true;
    }
}

void LeaderboardViewLayer::setupStatBtns() {
    for(const auto& btn : m_statBtns) {
        btn->removeFromParent();
    }
    m_statBtns.clear();

    for(const auto& btn : m_topBtns) {
        btn->removeFromParent();
    }
    m_topBtns.clear();

    if(m_mode == BILeaderboardMode::Top1000) {
        auto top1000 = TabButton::create("Top 1000", this, menu_selector(LeaderboardViewLayer::onTabChanged));
        auto friends = TabButton::create("Friends", this, menu_selector(LeaderboardViewLayer::onTabChanged));
        auto creator = TabButton::create("Creators", this, menu_selector(LeaderboardViewLayer::onTabChanged));

        top1000->setTag((int) BILeaderboardTab::Global);
        friends->setTag((int) BILeaderboardTab::Friends);
        creator->setTag((int) BILeaderboardTab::Creator);

        top1000->toggle(m_tab == BILeaderboardTab::Global);
        friends->toggle(m_tab == BILeaderboardTab::Friends);
        creator->toggle(m_tab == BILeaderboardTab::Creator);

        m_topBtns.push_back(top1000);
        m_topBtns.push_back(friends);
        m_topBtns.push_back(creator);

        m_topMenu->addChild(top1000);
        m_topMenu->addChild(friends);
        m_topMenu->addChild(creator);

        m_topMenu->updateLayout();

        // tab gradient code stolen from DANK MEME by GLOBED 3
        if(m_tabsClipper) m_tabsClipper->removeFromParent();
        if(m_tabsGradientSpr) m_tabsGradientSpr->removeFromParent();
        if(m_tabsGradientStencil) m_tabsGradientStencil->removeFromParent();

        m_tabsClipper = CCClippingNode::create();
        m_tabsClipper->setID("gradient-clipping-node"_spr);
        m_tabsClipper->setContentSize(this->getContentSize());
        m_tabsClipper->setAnchorPoint({0.5f, 0.5f});
        m_tabsClipper->ignoreAnchorPointForPosition(true);
        m_tabsClipper->setZOrder(-1);
        m_tabsClipper->setInverted(false);
        m_tabsClipper->setAlphaThreshold(0.7f);

        m_tabsGradientSpr = CCSprite::create("tab-gradient.png"_spr);
        m_tabsGradientSpr->setPosition(m_topMenu->getPosition());
        m_tabsClipper->addChild(m_tabsGradientSpr);

        m_tabsGradientStencil = CCSprite::create("tab-gradient-mask.png"_spr);
        m_tabsGradientStencil->setZOrder(-10);
        m_tabsGradientStencil->setScale(1.025f);
        m_tabsGradientStencil->setAnchorPoint({0.f, 0.f});
        m_tabsClipper->setStencil(m_tabsGradientStencil);

        this->addChild(m_tabsClipper);
        this->addChild(m_tabsGradientStencil);
        // end of tab gradient code stolen from DANK MEME by GLOBED 3

        for(auto btn : m_topBtns) {
            for(auto btn2 : std::array<CCMenuItemSpriteExtra*, 2>{btn->m_offButton, btn->m_onButton}) {
                btn2->m_colorDip = .8f;
                btn2->m_colorEnabled = true;
                btn2->m_scaleMultiplier = 1.f;
                btn2->m_animationEnabled = false;
            }
            if (btn->isToggled()) {
                m_tabsGradientStencil->setPosition(btn->m_onButton->convertToWorldSpace({}));
            }
        }
    }

    if(m_tab == BILeaderboardTab::Global || m_tab == BILeaderboardTab::Friends) {
        std::vector<std::tuple<const char*, const char*, int>> stats = {
            std::make_tuple("GJ_starsIcon_001.png", "stars", 0),
            std::make_tuple("GJ_moonsIcon_001.png", "moons", 1),
            std::make_tuple("GJ_demonIcon_001.png", "demons", 2),
            
        };

        if(m_tab == BILeaderboardTab::Friends) {
            stats.push_back(std::make_tuple("GJ_hammerIcon_001.png", "creator-points", 4));
            stats.push_back(std::make_tuple("GJ_sRecentIcon_001.png", "time-added", 5));
        } else {
            stats.push_back(std::make_tuple("GJ_coinsIcon2_001.png", "user-coins", 3));
        }

        for (const auto& [iconName, statName, statId] : stats) {
            auto icon = CCSprite::createWithSpriteFrameName(iconName);
            auto sprite = ButtonSprite::create(icon, 32, 0, 320.0, 1.0f, true, m_stat == statId ? "GJ_button_02.png" : "GJ_button_01.png", false);
            sprite->updateSpriteOffset({0, -1.5f});
            sprite->setScale(0.6f);
            icon->setScale(1.2f);
            auto btn = CCMenuItemExt::createSpriteExtra(
                sprite,
                [this, statId] (auto sprite) {
                    this->loadStat(statId, false);
                }
            );
            btn->setID(fmt::format("{}-button"_spr, statName));
            btn->setSizeMult(1.5f);
            m_rightMenu->addChild(btn);
            m_statBtns.push_back(btn);
        }

        if(m_tab == BILeaderboardTab::Friends) {
            auto lastBtn = m_statBtns.back();
            if(auto lastSprite = typeinfo_cast<ButtonSprite*>(lastBtn->getNormalImage())) {
                lastSprite->m_subSprite->setScale(1.6f);
            }
        }

        m_rightMenu->updateLayout();
    }
}

void LeaderboardViewLayer::loadPage(){
    if(!m_data) return;

    m_listView = CustomListView::create(m_data, BoomListType::Score, 220.f, 356.f);
    BIViewLayer::loadPage();
}

void LeaderboardViewLayer::keyBackClicked() {
    BIViewLayer::keyBackClicked();
}

void LeaderboardViewLayer::onRefresh(CCObject* object) {
    this->loadStat(m_stat, true); 
}

void LeaderboardViewLayer::loadFriends(int stat, bool reload) {
    if(m_tab != BILeaderboardTab::Friends) return;

    auto getSortableStat = [stat] (GJUserScore* score) -> int {
        switch(stat) {
            case 0: return score->m_stars;
            case 1: return score->m_moons;
            case 2: return score->m_demons;
            case 3: return score->m_userCoins;
            case 4: return score->m_creatorPoints;
            case 5: {
                if(auto time = typeinfo_cast<CCInteger*>(score->getUserObject("key_64"_spr))) {
                    return - time->getValue();
                }
                return 0;
            }
            default: return 0;
        }
    };

    if(!reload) {
        if(auto friends = typeinfo_cast<CCArray*>(GameLevelManager::sharedState()->m_storedLevels->objectForKey("get_friends"))) {
            //auto friendsCopy = typeinfo_cast<CCArray*>(friends->copy())->asExt<GJUserScore*>();
            auto friendsCopy = CCArray::create()->asExt<GJUserScore*>();
            for(auto score : friends->asExt<GJUserScore*>()) {
                friendsCopy.push_back(score);
            }
            friendsCopy.push_back(BetterInfo::createSelfScore());

            std::sort(friendsCopy.begin(), friendsCopy.end(), [getSortableStat] (GJUserScore* a, GJUserScore* b) {
                return getSortableStat(a) > getSortableStat(b);
            });

            int rank = stat == 5 ? 0 : 1;
            for(auto score : friendsCopy) {
                score->m_playerRank = rank++;
            }

            onLeaderboardFinished(friendsCopy.inner(), stat);
            return;
        }
    }

    auto GLM = GameLevelManager::sharedState();
    GLM->m_userListDelegate = this;
    GLM->getUserList(UserListType::Friends);
}

void LeaderboardViewLayer::loadStat(int stat, bool reload) {
    setData(CCArray::create());
    loadPage();

    showCircle();
    m_stat = stat;

    setupStatBtns();

    if(m_mode == BILeaderboardMode::Account) {
        BetterInfoOnline::sharedState()->loadScores(m_accountID, reload, this, nullptr, stat);
    } else if(m_mode == BILeaderboardMode::Top1000) {
        if(m_tab == BILeaderboardTab::Friends) {
            loadFriends(stat, reload);
        } else {
            async::spawn(
                BetterInfoOnline::sharedState()->loadGlobalScores(m_tab == BILeaderboardTab::Creator ? LeaderboardType::Creator : LeaderboardType::Top100, (LeaderboardStat) stat, reload), 
                [self = Ref(this), stat] (CCArray* scores) { self->onLeaderboardFinished(scores, stat); }
            );
        }
    }
}

CCScene* LeaderboardViewLayer::scene(BILeaderboardMode mode, int accountID) {
    auto layer = LeaderboardViewLayer::create(mode, accountID);
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}

void LeaderboardViewLayer::onLeaderboardFinished(cocos2d::CCArray* scores, int stat) {
    if(stat != m_stat) return;

    setData(scores);
    loadPage();
    hideCircle();
}

LeaderboardViewLayer::~LeaderboardViewLayer() {
    BetterInfoOnline::sharedState()->cancelScoresRequest(this);

    auto GLM = GameLevelManager::sharedState();
    if(GLM->m_userListDelegate == this) {
        GLM->m_userListDelegate = nullptr;
    }
}

void LeaderboardViewLayer::getUserListFinished(cocos2d::CCArray* scores, UserListType type) {
    if(type == UserListType::Friends) {
        loadFriends(m_stat, false);
    }
}

void LeaderboardViewLayer::getUserListFailed(UserListType type, GJErrorCode errorType) {
    if(type == UserListType::Friends) {
        setData(CCArray::create());
        loadPage();
        hideCircle();
    }
}