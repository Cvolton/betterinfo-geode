#include "BIBaseLayer.h"

BIBaseLayer* BIBaseLayer::create(bool BL, bool BR, bool TL, bool TR) {
    auto ret = new BIBaseLayer();
    if (ret && ret->init(BL, BR, TL, TR)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool BIBaseLayer::init(bool BL, bool BR, bool TL, bool TR) {
    auto backgroundSprite = CCSprite::create("game_bg_14_001.png");
    backgroundSprite->setID("background"_spr);
    
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
    backBtn->setID("exit-button"_spr);

    auto menuBack = CCMenu::create();
    menuBack->addChild(backBtn);
    menuBack->setPosition({25, winSize.height - 25});
    menuBack->setID("exit-menu"_spr);

    m_cornerBL = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    m_cornerBL->setPosition({0,0});
    m_cornerBL->setAnchorPoint({0,0});
    addChild(m_cornerBL, -1);
    m_cornerBL->setID("corner-bl"_spr);

    m_cornerTL = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    m_cornerTL->setPosition({0,winSize.height});
    m_cornerTL->setAnchorPoint({0,1});
    m_cornerTL->setFlipY(true);
    m_cornerTL->setID("corner-ul"_spr);
    addChild(m_cornerTL, -1);

    m_cornerTR = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    m_cornerTR->setPosition({winSize.width,winSize.height});
    m_cornerTR->setAnchorPoint({1,1});
    m_cornerTR->setFlipX(true);
    m_cornerTR->setFlipY(true);
    m_cornerTR->setID("corner-ur"_spr);
    addChild(m_cornerTR, -1);

    m_cornerBR = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    m_cornerBR->setPosition({winSize.width,0});
    m_cornerBR->setAnchorPoint({1,0});
    m_cornerBR->setFlipX(true);
    m_cornerBR->setID("corner-br"_spr);
    addChild(m_cornerBR, -1);

    addChild(menuBack);

    setKeypadEnabled(true);
    setCorners(BL, BR, TL, TR);

    return true;
}

void BIBaseLayer::setCorners(bool BL, bool BR, bool TL, bool TR) {
    m_cornerBL->setVisible(BL);
    m_cornerBR->setVisible(BR);
    m_cornerTL->setVisible(TL);
    m_cornerTR->setVisible(TR);
}

void BIBaseLayer::keyBackClicked() {
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}


void BIBaseLayer::onBack(CCObject* object) {
    keyBackClicked();
}

CCScene* BIBaseLayer::scene(bool BL, bool BR, bool TL, bool TR) {
    auto layer = BIBaseLayer::create(BL, BR, TL, TR);
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}
