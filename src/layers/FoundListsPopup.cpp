#include "FoundListsPopup.h"
#include "../utils/TimeUtils.h"
#include "../utils/ParticleHelper.h"

#include "../managers/BetterInfoCache.h"

FoundListsPopup* FoundListsPopup::create(){
    auto ret = new FoundListsPopup();
    if (ret && ret->init()) {
        //robert 1 :D
        ret->autorelease();
    } else {
        //robert -1
        delete ret;
        ret = nullptr;
    }
    return ret;
}

void FoundListsPopup::onClose(cocos2d::CCObject* sender)
{
    CvoltonAlertLayerStub::onClose(sender);
}

void FoundListsPopup::onOK(cocos2d::CCObject* sender){
    //TODO: BetterInfoCache
    //BetterInfoCache::sharedState()->showClaimableLists();
    this->onClose(sender);
}

bool FoundListsPopup::init(){
    if(!CvoltonAlertLayerStub::init({230.0f, 160.0f}, 0.8f, 0x4B, 2)) return false;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    //TODO: BetterInfoCache
    /*auto textLabel = TextArea::create(fmt::format("You can claim rewards\nfrom <cg>{}</c> new lists!", BetterInfoCache::sharedState()->claimableListsCount()), "chatFont.fnt", 1, 295, {0.5,0.5}, 20, false);
    textLabel->setAnchorPoint({0.5,0.5});
    textLabel->setPosition({0,1});
    textLabel->setID("text-label"_spr);
    m_buttonMenu->addChild(textLabel);*/

    cocos2d::extension::CCScale9Sprite* infoBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f });
    infoBg->setContentSize({320,114});
    infoBg->setColor({50,50,100});
    m_buttonMenu->addChild(infoBg, -1);
    infoBg->setPosition({0,0});
    infoBg->setScale(0.6f);
    infoBg->setID("info-background"_spr);

    auto buttonSprite = ButtonSprite::create("Go", 40, true, "goldFont.fnt", "GJ_button_01.png", 30, 0.8f);
    auto goBtn = CCMenuItemSpriteExtra::create(
        buttonSprite,
        this,
        menu_selector(FoundListsPopup::onOK)
    );
    goBtn->setSizeMult(1.2f);
    goBtn->setPosition({0,-55});
    goBtn->setID("go-button"_spr);
    m_buttonMenu->addChild(goBtn);

    auto claimIcon3 = CCSprite::createWithSpriteFrameName("GJ_bigDiamond_001.png");
    claimIcon3->setPosition({90, 56});
    claimIcon3->setScale(0.775f);
    claimIcon3->setZOrder(4);
    m_buttonMenu->addChild(claimIcon3);

    auto claimIcon4 = CCSprite::createWithSpriteFrameName("rewardsLabel_001.png");
    claimIcon4->setPosition({-5, 55});
    claimIcon4->setScale(0.725f);
    claimIcon4->setZOrder(4);
    m_buttonMenu->addChild(claimIcon4);

    auto particles = ParticleHelper::createStarParticles(7);
    particles->setPosition(claimIcon3->getPosition());
    particles->setAnchorPoint({ 0.5f, 0.5f });
    particles->setScale(.8f);
    particles->setZOrder(3);
    m_buttonMenu->addChild(particles);

    return true;
}

CCLabelBMFont* FoundListsPopup::createTextLabel(const std::string text, const CCPoint& position, const float scale, CCNode* menu, const char* font){
    CCLabelBMFont* bmFont = CCLabelBMFont::create(text.c_str(), font);
    bmFont->setPosition(position);
    bmFont->setScale(scale);
    menu->addChild(bmFont);
    return bmFont;
}