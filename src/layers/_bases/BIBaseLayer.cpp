#include "BIBaseLayer.h"

BIBaseLayer* BIBaseLayer::create() {
    auto ret = new BIBaseLayer();
    if (ret && ret->init()) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool BIBaseLayer::init() {
    auto backgroundSprite = CCSprite::create("game_bg_14_001.png");
    
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto size = backgroundSprite->getContentSize();
    
    backgroundSprite->setScale(winSize.width / size.width);
    
    backgroundSprite->setAnchorPoint({0, 0});
    backgroundSprite->setPosition({0,-75});
    
    backgroundSprite->setColor({164, 0, 255});
    
    backgroundSprite->setZOrder(-2);
    addChild(backgroundSprite);

    auto backBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png"),
        this,
        menu_selector(BIBaseLayer::onBack)
    );

    auto menuBack = CCMenu::create();
    menuBack->addChild(backBtn);
    menuBack->setPosition({25, winSize.height - 25});

    addChild(menuBack);

    setKeypadEnabled(true);

    return true;
}

void BIBaseLayer::keyBackClicked() {
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}


void BIBaseLayer::onBack(CCObject* object) {
    keyBackClicked();
}

CCScene* BIBaseLayer::scene() {
    auto layer = BIBaseLayer::create();
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}
