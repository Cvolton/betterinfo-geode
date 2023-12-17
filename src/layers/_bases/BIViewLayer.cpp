#include "BIViewLayer.h"
#include "../JumpToPageLayer.h"
#include "../../utils.hpp"

BIViewLayer* BIViewLayer::create() {
    auto ret = new BIViewLayer();
    if (ret && ret->init()) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool BIViewLayer::init() {
    BIBaseLayer::init();

    auto controllerConnected = BetterInfo::controllerConnected();
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto menu = CCMenu::create();
    auto prevSprite = CCSprite::createWithSpriteFrameName(controllerConnected ? "controllerBtn_DPad_Left_001.png" : "GJ_arrow_03_001.png");
    m_prevBtn = CCMenuItemSpriteExtra::create(
        prevSprite,
        this,
        menu_selector(BIViewLayer::onPrev)
    );
    m_prevBtn->setPosition({- (winSize.width / 2) + 25, 0});
    menu->addChild(m_prevBtn);

    auto nextSprite = CCSprite::createWithSpriteFrameName(controllerConnected ? "controllerBtn_DPad_Right_001.png" : "GJ_arrow_03_001.png");
    if(!controllerConnected) nextSprite->setFlipX(true);
    m_nextBtn = CCMenuItemSpriteExtra::create(
        nextSprite,
        this,
        menu_selector(BIViewLayer::onNext)
    );
    m_nextBtn->setPosition({+ (winSize.width / 2) - 25, 0});
    menu->addChild(m_nextBtn);

    m_counter = CCLabelBMFont::create("0 to 0 of 0", "goldFont.fnt");
    m_counter->setAnchorPoint({ 1.f, 1.f });
    m_counter->setPosition(winSize - CCPoint(7,3));
    m_counter->setScale(0.5f);
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
    menu->addChild(m_pageBtn);

    auto randomSprite = BetterInfo::createWithBISpriteFrameName("BI_randomBtn_001.png");
    randomSprite->setScale(0.9f);
    m_randomBtn = CCMenuItemSpriteExtra::create(
        randomSprite,
        this,
        menu_selector(BIViewLayer::onRandom)
    );
    m_randomBtn->setPosition({ (winSize.width / 2) - 23, (winSize.height / 2) - 72});
    menu->addChild(m_randomBtn);

    addChild(menu);

    loadPage(0);
    return true;
}

void BIViewLayer::loadPage(unsigned int page){
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    const unsigned int count = resultsPerPage();
    unsigned int firstIndex = page * count;
    unsigned int lastIndex = (page+1) * count;

    m_page = page;

    if(page == 0) m_prevBtn->setVisible(false);
    else m_prevBtn->setVisible(true);

    if(m_data->count() > lastIndex) m_nextBtn->setVisible(true);
    else m_nextBtn->setVisible(false);

    if(m_data->count() > resultsPerPage()) m_pageBtn->setVisible(true);
    else m_pageBtn->setVisible(false);

    if(m_data->count() > resultsPerPage()) m_randomBtn->setVisible(true);
    else m_randomBtn->setVisible(false);

    m_pageBtnSprite->setString(std::to_string(page+1).c_str());

    if(m_listLayer != nullptr) m_listLayer->removeFromParentAndCleanup(true);
    
    m_listLayer = GJListLayer::create(m_listView, m_title.c_str(), {191, 114, 62, 255}, 356.f, 220.f);
    m_listLayer->setPosition(winSize / 2 - m_listLayer->getScaledContentSize() / 2 - CCPoint(0,5));
    addChild(m_listLayer);

    m_counter->setCString(CCString::createWithFormat("%i to %i of %i", firstIndex+1, (m_data->count() >= lastIndex) ? lastIndex : m_data->count(), m_data->count())->getCString());
}

void BIViewLayer::keyBackClicked() {
    setTouchEnabled(false);
    setKeypadEnabled(false);
    m_data->release();
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

void BIViewLayer::onJumpToPageLayer(CCObject* sender){
    JumpToPageLayer::create(this)->show();
}

void BIViewLayer::onRandom(CCObject* sender){
    loadPage(BetterInfo::randomNumber(0, m_data->count() / resultsPerPage()));
}

CCScene* BIViewLayer::scene() {
    auto layer = BIViewLayer::create();
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}

int BIViewLayer::getPage() const{
    return m_page;
}

int BIViewLayer::resultsPerPage() const{
    return (GameManager::sharedState()->getGameVariable("0093")) ? 20 : 10;
}

void BIViewLayer::keyDown(enumKeyCodes key){
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