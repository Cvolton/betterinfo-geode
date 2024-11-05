#include "DailyViewLayer.h"

DailyViewLayer* DailyViewLayer::create(GJTimedLevelType timedType) {
    auto ret = new DailyViewLayer();
    if (ret && ret->init(timedType)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool DailyViewLayer::compareDailies(const void* l1, const void* l2){
    const GJGameLevel* level1 = reinterpret_cast<const GJGameLevel*>(l1);
    const GJGameLevel* level2 = reinterpret_cast<const GJGameLevel*>(l2);
    return const_cast<geode::SeedValueRSV&>(level1->m_dailyID).value() < const_cast<geode::SeedValueRSV&>(level2->m_dailyID).value();
}

bool DailyViewLayer::init(GJTimedLevelType timedType) {
    m_timedType = timedType;

    // initialize data
    auto GLM = GameLevelManager::sharedState();
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    setData(CCArray::create());

    for(auto [key, level] : CCDictionaryExt<gd::string, GJGameLevel*>(GLM->m_dailyLevels)){
        if(level == nullptr) continue;

        if(timedType == GJTimedLevelType::Event && level->m_dailyID >= 200000){
            m_data->addObject(level);
        } else if(timedType == GJTimedLevelType::Weekly && level->m_dailyID >= 100000 && level->m_dailyID < 200000){
            m_data->addObject(level);
        } else if(timedType == GJTimedLevelType::Daily && level->m_dailyID < 100000){
            m_data->addObject(level);
        }
    }

    std::sort(m_data->data->arr, m_data->data->arr + m_data->data->num, DailyViewLayer::compareDailies);
    
    // set metadata
    switch(m_timedType){
        default:
        case GJTimedLevelType::Daily:
            m_title = "Daily Levels";
            break;
        case GJTimedLevelType::Weekly:
            m_title = "Weekly Demons";
            break;
        case GJTimedLevelType::Event:
            m_title = "Event Levels";
            break;
    }

    m_noInternetText = fmt::format("You have not <cg>played</c> any\n<cl>{}</c> yet!", m_title);
    m_showNoInternet = true;

    // init the layer
    BIViewLayer::init();

    // add more btn
    auto menu = CCMenu::create();
    menu->setID("more-menu"_spr);

    auto buttonSprite = ButtonSprite::create("More", (int)(90*0.5), true, "bigFont.fnt", "GJ_button_01.png", 44*0.5f, 0.5f);
    auto buttonButton = CCMenuItemSpriteExtra::create(
        buttonSprite,
        this,
        menu_selector(DailyViewLayer::onMore)
    );
    buttonButton->setSizeMult(1.2f);
    buttonButton->setPosition({- winSize.width / 2, - winSize.height / 2});
    buttonButton->setAnchorPoint({0,0});
    buttonButton->setID("more-button"_spr);
    menu->addChild(buttonButton);

    addChild(menu);

    return true;
}

void DailyViewLayer::loadPage(){
    auto displayedLevels = trimData();

    m_listView = CvoltonListView<DailyCell>::create(displayedLevels, 356.f, 220.f);

    BIViewLayer::loadPage();
}

GJSearchObject* DailyViewLayer::getSearchObject() {
    switch(m_timedType){
        default:
        case GJTimedLevelType::Daily:
            return GJSearchObject::create(SearchType::DailySafe);
        case GJTimedLevelType::Weekly:
            return GJSearchObject::create(SearchType::WeeklySafe);
        case GJTimedLevelType::Event:
            return GJSearchObject::create(SearchType::EventSafe);
    }
}

void DailyViewLayer::onMore(CCObject* object) {
    auto browserLayer = LevelBrowserLayer::scene(getSearchObject());

    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);

    CCDirector::sharedDirector()->pushScene(transitionFade);
}

CCScene* DailyViewLayer::scene(GJTimedLevelType timedType) {
    auto layer = DailyViewLayer::create(timedType);
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}