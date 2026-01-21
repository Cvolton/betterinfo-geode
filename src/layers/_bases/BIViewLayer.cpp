#include "BIViewLayer.h"
#include "../JumpToPageLayer.h"
#include "../../utils.hpp"
#include "../../ui/DoubleArrow.h"

BIViewLayer* BIViewLayer::create(bool paginated) {
    auto ret = new BIViewLayer();
    if (ret && ret->init(paginated)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool BIViewLayer::init(bool paginated) {
    BIBaseLayer::init(true, true, false, false);

    m_paginated = paginated;

    auto controllerConnected = BetterInfo::controllerConnected();
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto menu = CCMenu::create();
    menu->setID("view-layer-menu"_spr);
    auto prevSprite = CCSprite::createWithSpriteFrameName(controllerConnected ? "controllerBtn_DPad_Left_001.png" : "GJ_arrow_03_001.png");
    m_prevBtn = CCMenuItemSpriteExtra::create(
        prevSprite,
        this,
        menu_selector(BIViewLayer::onPrev)
    );
    m_prevBtn->setPosition({- (winSize.width / 2) + 25, 0});
    m_prevBtn->setID("prev-button"_spr);
    m_prevBtn->setSizeMult(1.2f);
    menu->addChild(m_prevBtn);

    auto nextSprite = CCSprite::createWithSpriteFrameName(controllerConnected ? "controllerBtn_DPad_Right_001.png" : "GJ_arrow_03_001.png");
    if(!controllerConnected) nextSprite->setFlipX(true);
    m_nextBtn = CCMenuItemSpriteExtra::create(
        nextSprite,
        this,
        menu_selector(BIViewLayer::onNext)
    );
    m_nextBtn->setPosition({+ (winSize.width / 2) - 25, 0});
    m_nextBtn->setID("next-button"_spr);
    m_nextBtn->setSizeMult(1.2f);
    menu->addChild(m_nextBtn);

    m_counter = CCLabelBMFont::create("0 to 0 of 0", "goldFont.fnt");
    m_counter->setAnchorPoint({ 1.f, 1.f });
    m_counter->setPosition(winSize - CCPoint(7,3));
    m_counter->setScale(0.5f);
    m_counter->setID("counter"_spr);
    addChild(m_counter);

    //navigation buttons
    m_pageBtnSprite = ButtonSprite::create("1", 23, true, "bigFont.fnt", "GJ_button_02.png", 40, .7f);
    m_pageBtnSprite->setScale(0.7f);
    m_pageBtn = CCMenuItemSpriteExtra::create(
        m_pageBtnSprite,
        this,
        menu_selector(BIViewLayer::onJumpToPageLayer)
    );
    m_pageBtn->setSizeMult(1.2f);
    m_pageBtn->setPosition({+ (winSize.width / 2) - 23, (winSize.height / 2) - 37});
    m_pageBtn->setID("page-button"_spr);
    menu->addChild(m_pageBtn);

    auto randomSprite = BetterInfo::createWithBISpriteFrameName("BI_randomBtn_001.png");
    randomSprite->setScale(0.9f);
    m_randomBtn = CCMenuItemSpriteExtra::create(
        randomSprite,
        this,
        menu_selector(BIViewLayer::onRandom)
    );
    m_randomBtn->setPosition({ (winSize.width / 2) - 23, (winSize.height / 2) - 72});
    m_randomBtn->setID("random-button"_spr);
    m_randomBtn->setSizeMult(1.2f);
    menu->addChild(m_randomBtn);

    m_firstBtn = CCMenuItemSpriteExtra::create(
        DoubleArrow::create(false),
        this,
        menu_selector(BIViewLayer::onFirst)
    );
    m_firstBtn->setID("first-button"_spr);
    m_firstBtn->setPosition({ - (winSize.width / 2) + 23, 60});
    m_firstBtn->setSizeMult(1.2f);
    menu->addChild(m_firstBtn);

    m_lastBtn = CCMenuItemSpriteExtra::create(
        DoubleArrow::create(true),
        this,
        menu_selector(BIViewLayer::onLast)
    );
    m_lastBtn->setPosition({ (winSize.width / 2) - 23, 60});
    m_lastBtn->setID("last-button"_spr);
    menu->addChild(m_lastBtn);

    m_noInternet = TextArea::create(m_noInternetText.c_str(), "bigFont.fnt", 1.0f, 600, {0.5f, 0.5f}, 40, false);
    m_noInternet->setPosition(winSize / 2);
    m_noInternet->setScale(0.6f);
    m_noInternet->setZOrder(20);
    m_noInternet->setVisible(false);
    m_noInternet->setID("no-internet-text"_spr);
    addChild(m_noInternet);

    if(!paginated) {
        m_pageBtn->setVisible(false);
        m_randomBtn->setVisible(false);
        m_counter->setVisible(false);
        m_lastBtn->setVisible(false);
        m_firstBtn->setVisible(false);
    }

    addChild(menu);

    loadPage(0);
    return true;
}

void BIViewLayer::loadPage(){
    if(!m_data) return;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    m_pageBtnSprite->setString(std::to_string(m_page+1).c_str());

    if(m_listLayer != nullptr) m_listLayer->removeFromParentAndCleanup(true);
    
    m_listLayer = GJListLayer::create(m_listView, m_title.c_str(), {191, 114, 62, 255}, 356.f, 220.f, 0);
    m_listLayer->setPosition(winSize / 2 - m_listLayer->getScaledContentSize() / 2 - CCPoint(0,5));
    m_listLayer->setID("list-layer"_spr);
    addChild(m_listLayer);

    m_noInternet->setVisible(m_showNoInternet && m_data->count() == 0);

    updateCounter();
}

void BIViewLayer::updateCounter(){
    if(!m_data) return;

    const unsigned int count = resultsPerPage();
    unsigned int firstIndex = m_page * count;
    unsigned int lastIndex = (m_page+1) * count;

    if(m_page == 0) {
        m_prevBtn->setVisible(false);
        m_firstBtn->setVisible(false);
    } else {
        m_prevBtn->setVisible(true);
        m_firstBtn->setVisible(true);
    }
    
    if(m_data->count() > lastIndex) {
        m_nextBtn->setVisible(true);
        m_lastBtn->setVisible(true);
    } else {
        m_nextBtn->setVisible(false);
        m_lastBtn->setVisible(false);
    }

    if(m_data->count() > count) {
        m_pageBtn->setVisible(true);
        m_randomBtn->setVisible(true);
    } else {
        m_pageBtn->setVisible(false);
        m_randomBtn->setVisible(false);
    }

    m_counter->setVisible(m_data->count() > 0);
    m_counter->setCString(CCString::createWithFormat("%i to %i of %i", firstIndex+1, (m_data->count() >= lastIndex) ? lastIndex : m_data->count(), m_data->count())->getCString());
}

void BIViewLayer::loadPage(unsigned int page){
    m_page = page;
    loadPage();
}

void BIViewLayer::keyBackClicked() {
    setTouchEnabled(false);
    setKeypadEnabled(false);

    m_circle = nullptr;
    
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}

CCArray* BIViewLayer::trimData(){
    CCArray* displayedLevels = CCArray::create();

    const unsigned int levelCount = resultsPerPage();
    unsigned int firstIndex = m_page * levelCount;
    unsigned int lastIndex = (m_page+1) * levelCount;

    for(unsigned int i = firstIndex; i < lastIndex; i++){
        auto levelObject = m_data->objectAtIndex(i);
        if(i >= m_data->count() || levelObject == nullptr) break;

        displayedLevels->addObject(levelObject);
    }

    return displayedLevels;
}

void BIViewLayer::onBack(CCObject* object) {
    keyBackClicked();
}

void BIViewLayer::onPrev(CCObject* object) {
    loadPage(--m_page);
}

void BIViewLayer::onNext(CCObject* object) {
    loadPage(++m_page);
}

void BIViewLayer::onFirst(CCObject* object) {
    loadPage(0);
}

void BIViewLayer::onLast(CCObject* object) {
    loadPage((m_data->count() - 1) / resultsPerPage());
}

void BIViewLayer::onJumpToPageLayer(CCObject* sender){
    JumpToPageLayer::create(this)->show();
}

void BIViewLayer::onRandom(CCObject* sender){
    loadPage(BetterInfo::randomNumber(0, m_data->count() / resultsPerPage()));
}

CCScene* BIViewLayer::scene(bool paginated) {
    auto layer = BIViewLayer::create(paginated);
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}

int BIViewLayer::getPage() const{
    return m_page;
}

int BIViewLayer::resultsPerPage() const{
    return !m_paginated ? m_data->count() : ((GameManager::sharedState()->getGameVariable("0093")) ? 20 : 10);
}

void BIViewLayer::keyDown(enumKeyCodes key, double timestamp){
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
            CCLayer::keyDown(key, timestamp);
    }
}

void BIViewLayer::setData(CCArray* data){
    m_data = data;
}

void BIViewLayer::showCircle(){
    hideCircle();

    m_circle = LoadingCircle::create();
    m_circle->setID("loading-circle"_spr);
    m_circle->setParentLayer(this);
    m_circle->show();
}

void BIViewLayer::hideCircle(){
    if(m_circle) {
        m_circle->fadeAndRemove();
        m_circle = nullptr;
    }
}