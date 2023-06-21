#include "RewardViewLayer.h"
#include "RewardListView.h"
#include "JumpToPageLayer.h"
#include "../utils.hpp"

using namespace gd;
using namespace cocos2d;

RewardViewLayer* RewardViewLayer::create(CCDictionary* chests, const char* title) {
    auto ret = new RewardViewLayer();
    if (ret && ret->init(chests, title)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool RewardViewLayer::compareRewards(const void* i1, const void* i2){
    const GJRewardItem* item1 = reinterpret_cast<const GJRewardItem*>(i1);
    const GJRewardItem* item2 = reinterpret_cast<const GJRewardItem*>(i2);
    return item1->m_chestID < item2->m_chestID;
}

bool RewardViewLayer::init(CCDictionary* chests, const char* title) {

    this->title = title;

    Mod::get()->getSavedValue<std::string>("reward-cell-title", title);;

    auto GSM = GameStatsManager::sharedState();
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
        menu_selector(RewardViewLayer::onBack)
    );

    backBtn->setPosition({(-winSize.width / 2) + 25, (winSize.height / 2) - 25});
    menu->addChild(backBtn);

    /*auto menuBack = CCMenu::create();
    menuBack->addChild(backBtn);
    menuBack->setPosition({25, winSize.height - 25});

    addChild(menuBack);*/

    setTouchEnabled(true);
    setKeypadEnabled(true);

    sortedRewards = CCArray::create();
    sortedRewards->retain();
    CCDictElement* obj;
    CCDICT_FOREACH(chests, obj){
        auto currentReward = static_cast<GJRewardItem*>(obj->getObject());
        if(currentReward != nullptr) sortedRewards->addObject(currentReward);
    }
    std::sort(sortedRewards->data->arr, sortedRewards->data->arr + sortedRewards->data->num, RewardViewLayer::compareRewards);

    auto prevSprite = CCSprite::createWithSpriteFrameName(controllerConnected ? "controllerBtn_DPad_Left_001.png" : "GJ_arrow_03_001.png");
    prevBtn = CCMenuItemSpriteExtra::create(
        prevSprite,
        this,
        menu_selector(RewardViewLayer::onPrev)
    );
    prevBtn->setPosition({- (winSize.width / 2) + 25, 0});
    menu->addChild(prevBtn);

    auto nextSprite = CCSprite::createWithSpriteFrameName(controllerConnected ? "controllerBtn_DPad_Right_001.png" : "GJ_arrow_03_001.png");
    if(!controllerConnected) nextSprite->setFlipX(true);
    nextBtn = CCMenuItemSpriteExtra::create(
        nextSprite,
        this,
        menu_selector(RewardViewLayer::onNext)
    );
    nextBtn->setPosition({+ (winSize.width / 2) - 25, 0});
    menu->addChild(nextBtn);

    counter = CCLabelBMFont::create("0 to 0 of 0", "goldFont.fnt");
    counter->setAnchorPoint({ 1.f, 1.f });
    counter->setPosition(winSize - CCPoint(7,3));
    counter->setScale(0.5f);
    addChild(counter);

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

    //navigation buttons
    pageBtnSprite = ButtonSprite::create("1", 23, true, "bigFont.fnt", "GJ_button_02.png", 40, .7f);
    pageBtnSprite->setScale(0.7f);
    auto pageBtn = CCMenuItemSpriteExtra::create(
        pageBtnSprite,
        this,
        menu_selector(RewardViewLayer::onJumpToPageLayer)
    );
    pageBtn->setSizeMult(1.2f);
    pageBtn->setPosition({+ (winSize.width / 2) - 23, (winSize.height / 2) - 37});
    menu->addChild(pageBtn);

    auto randomSprite = BetterInfo::createWithBISpriteFrameName("BI_randomBtn_001.png");
    randomSprite->setScale(0.9f);
    auto randomBtn = CCMenuItemSpriteExtra::create(
        randomSprite,
        this,
        menu_selector(RewardViewLayer::onRandom)
    );
    randomBtn->setPosition({ (winSize.width / 2) - 23, (winSize.height / 2) - 72});
    menu->addChild(randomBtn);

    loadPage(0);
    return true;
}

void RewardViewLayer::loadPage(unsigned int page){

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    if(listLayer != nullptr) listLayer->removeFromParentAndCleanup(true);

    this->page = page;
    CCArray* displayedLevels = CCArray::create();
    //TODO: can we clone this by passing an iterator or something like that
    const unsigned int levelCount = rewardsPerPage();
    unsigned int firstIndex = page * levelCount;
    unsigned int lastIndex = (page+1) * levelCount;

    for(unsigned int i = firstIndex; i < lastIndex; i++){
        auto levelObject = sortedRewards->objectAtIndex(i);
        if(i >= sortedRewards->count() || levelObject == nullptr) break;

        displayedLevels->addObject(levelObject);
    }

    rewardView = RewardListView::create(displayedLevels, 356.f, 220.f);
    listLayer = GJListLayer::create(rewardView, CCString::createWithFormat("%s Chests", title)->getCString(), {191, 114, 62, 255}, 356.f, 220.f);
    listLayer->setPosition(winSize / 2 - listLayer->getScaledContentSize() / 2 - CCPoint(0,5));
    addChild(listLayer);

    if(page == 0) prevBtn->setVisible(false);
    else prevBtn->setVisible(true);

    if(sortedRewards->count() > lastIndex) nextBtn->setVisible(true);
    else nextBtn->setVisible(false);

    pageBtnSprite->setString(std::to_string(page+1).c_str());

    counter->setCString(CCString::createWithFormat("%i to %i of %i", firstIndex+1, (sortedRewards->count() >= lastIndex) ? lastIndex : sortedRewards->count(), sortedRewards->count())->getCString());
}

void RewardViewLayer::keyBackClicked() {
    setTouchEnabled(false);
    setKeypadEnabled(false);
    sortedRewards->release();
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}


void RewardViewLayer::onBack(CCObject* object) {
    keyBackClicked();
}

void RewardViewLayer::onPrev(CCObject* object) {
    loadPage(--page);
}

void RewardViewLayer::onNext(CCObject* object) {
    loadPage(++page);
}

void RewardViewLayer::onJumpToPageLayer(CCObject* sender){
    //JumpToPageLayer::create(this)->show();
}

void RewardViewLayer::onRandom(CCObject* sender){
    loadPage(BetterInfo::randomNumber(0, sortedRewards->count() / rewardsPerPage()));
}

CCScene* RewardViewLayer::scene(CCDictionary* chests, const char* title) {
    auto layer = RewardViewLayer::create(chests, title);
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}

int RewardViewLayer::getPage() const{
    return page;
}

int RewardViewLayer::rewardsPerPage() const{
    return (GameManager::sharedState()->getGameVariable("0093")) ? 20 : 10;
}

void RewardViewLayer::keyDown(enumKeyCodes key){
    switch(key){
        case KEY_Left:
        case CONTROLLER_Left:
            if(prevBtn->isVisible() == true) onPrev(nullptr);
            break;
        case KEY_Right:
        case CONTROLLER_Right:
            if(nextBtn->isVisible() == true) onNext(nullptr);
            break;
        default:
            CCLayer::keyDown(key);
    }
}