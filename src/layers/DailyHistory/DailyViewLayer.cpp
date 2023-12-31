#include "DailyViewLayer.h"
#include "../JumpToPageLayer.h"
#include "../../utils.hpp"

DailyViewLayer* DailyViewLayer::create(bool isWeekly) {
    auto ret = new DailyViewLayer();
    if (ret && ret->init(isWeekly)) {
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

bool DailyViewLayer::init(bool isWeekly) {
    // initialize data
    auto GLM = GameLevelManager::sharedState();
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    setData(CCArray::create());

    auto dailyLevels = GLM->m_dailyLevels;
    CCDictElement* obj;
    CCDICT_FOREACH(dailyLevels, obj){
        auto currentLvl = static_cast<GJGameLevel*>(obj->getObject());
        if(
            currentLvl != nullptr &&
            ((isWeekly && currentLvl->m_dailyID >= 100000) || (!isWeekly && currentLvl->m_dailyID < 100000))
        ){
            m_data->addObject(currentLvl);
        }
    }
    std::sort(m_data->data->arr, m_data->data->arr + m_data->data->num, DailyViewLayer::compareDailies);

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
    m_title = m_isWeekly ? "Weekly Demons" : "Daily Levels";

    BIViewLayer::loadPage();
}

void DailyViewLayer::onMore(CCObject* object) {
    auto searchObject = GJSearchObject::create(m_isWeekly ? SearchType::WeeklySafe : SearchType::DailySafe);
    auto browserLayer = LevelBrowserLayer::scene(searchObject);

    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);

    CCDirector::sharedDirector()->pushScene(transitionFade);
}

CCScene* DailyViewLayer::scene(bool isWeekly) {
    auto layer = DailyViewLayer::create(isWeekly);
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}