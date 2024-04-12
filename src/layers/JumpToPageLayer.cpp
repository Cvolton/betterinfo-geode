#include "JumpToPageLayer.h"

#include "../utils.hpp"
#include "LevelBrowserEndLayer.h"

JumpToPageLayer* JumpToPageLayer::create(InfoLayer* infoLayer){
    auto ret = new JumpToPageLayer();
    if (ret && ret->init(infoLayer)) {
        //robert 1 :D
        ret->autorelease();
    } else {
        //robert -1
        delete ret;
        ret = nullptr;
    }
    return ret;
}

JumpToPageLayer* JumpToPageLayer::create(PageNumberDelegate* pageNumberDelegate){
    auto ret = new JumpToPageLayer();
    if (ret && ret->init(pageNumberDelegate)) {
        //robert 1 :D
        ret->autorelease();
    } else {
        //robert -1
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool JumpToPageLayer::init(InfoLayer* infoLayer) {
    m_infoLayer = infoLayer;
    return init();
}

bool JumpToPageLayer::init(PageNumberDelegate* pageNumberDelegate) {
    m_pageNumberDelegate = pageNumberDelegate;
    return init();
}

void JumpToPageLayer::onClose(cocos2d::CCObject* sender)
{
    auto GLM = GameLevelManager::sharedState();
    if(GLM->m_levelCommentDelegate == this) GLM->m_levelCommentDelegate = nullptr;
    
    CvoltonAlertLayerStub::onClose(sender);
}

void JumpToPageLayer::onPrev(cocos2d::CCObject* sender)
{
    int newPage = pageNumber() - 1;
    if(newPage < 1) newPage = 1;
    m_textNode->setString(std::to_string(newPage).c_str());
}

void JumpToPageLayer::onNext(cocos2d::CCObject* sender)
{
    m_textNode->setString(std::to_string(pageNumber() + 1).c_str());
}

void JumpToPageLayer::onReset(cocos2d::CCObject* sender)
{
    m_textNode->setString("1");
}

int JumpToPageLayer::pageNumber(){
    return BetterInfo::stoi(m_textNode->getString());
}

void JumpToPageLayer::onLast(cocos2d::CCObject* sender)
{
    if(!m_infoLayer) return;

    if(m_infoLayer->m_itemCount == 999) {
        LevelBrowserEndLayer::create(nullptr, m_infoLayer)->show();
        onClose(sender);
        return;
    }

    m_infoLayer->loadPage(m_infoLayer->m_itemCount / m_infoLayer->m_pageEndIdx, false);
    onClose(sender);
}

void JumpToPageLayer::loadCommentsFinished(cocos2d::CCArray*, char const*) {
    if(!m_infoLayer) return;

    m_infoLayer->loadPage(0, false);
    m_lastBtn->setVisible(true);
    m_lastArrow->usePopupTexture(m_infoLayer->m_itemCount == 999);

    //workaround bc infolayer calls handlewith and handlewith doesnt rly detect that it did that
    this->setTouchPriority(-600);
}

void JumpToPageLayer::loadCommentsFailed(char const*) {
    if(!m_infoLayer) return;

    Notification::create("Failed to load comments.")->show();
}

void JumpToPageLayer::onOK(cocos2d::CCObject* sender){
    if(m_infoLayer != nullptr) m_infoLayer->loadPage(pageNumber()-1, false);
    if(m_pageNumberDelegate != nullptr) m_pageNumberDelegate->loadPage(pageNumber()-1);
    onClose(sender);
}

bool JumpToPageLayer::init(){
    if(!CvoltonAlertLayerStub::init({220.0f, 150.0f}, 0.8f)) return false;

    createTextLabel("Go to Page", {0,57}, 0.7f, m_buttonMenu, "goldFont.fnt")->setID("header-text"_spr);

    m_textNode = CCTextInputNode::create(50, 30, "Num", "bigFont.fnt");
    m_textNode->setLabelPlaceholderColor({0x75, 0xAA, 0xF0});
    if(m_infoLayer != nullptr) m_textNode->setString(std::to_string(m_infoLayer->m_page+1).c_str());
    if(m_pageNumberDelegate != nullptr) m_textNode->setString(std::to_string(m_pageNumberDelegate->getPage()+1).c_str());
    m_textNode->setAllowedChars("0123456789");
    m_textNode->setMaxLabelScale(0.7f);
    m_textNode->setMaxLabelWidth(40);
    m_textNode->setPosition({0,6});
    m_textNode->setID("text-input"_spr);
    m_buttonMenu->addChild(m_textNode);

    cocos2d::extension::CCScale9Sprite* infoBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f });
    infoBg->setContentSize({100,60});
    infoBg->setColor({123,60,31});
    m_buttonMenu->addChild(infoBg, -1);
    infoBg->setPosition({0,6});
    infoBg->setScale(0.6f);
    infoBg->setID("text-input-bg"_spr);

    auto buttonSprite = ButtonSprite::create("Go", 40, true, "goldFont.fnt", "GJ_button_01.png", 30, 0.8f);
    auto buttonButton = CCMenuItemSpriteExtra::create(
        buttonSprite,
        this,
        menu_selector(JumpToPageLayer::onOK)
    );
    buttonButton->setSizeMult(1.2f);
    buttonButton->setPosition({0,-50});
    buttonButton->setID("go-button"_spr);
    m_buttonMenu->addChild(buttonButton);

    createButton("edit_leftBtn_001.png", {-50, 6}, menu_selector(JumpToPageLayer::onPrev), 1.1f)->setID("left-arrow"_spr);
    createButton("edit_rightBtn_001.png", {50, 6}, menu_selector(JumpToPageLayer::onNext), 1.1f)->setID("right-arrow"_spr);
    createButton("GJ_resetBtn_001.png", {93, 57}, menu_selector(JumpToPageLayer::onReset))->setID("reset-button"_spr);

    if(m_infoLayer) {
        m_lastBtn = CCMenuItemSpriteExtra::create(
            m_lastArrow = DoubleArrow::create(true, m_infoLayer->m_itemCount == 999 ? "GJ_arrow_03_001.png" : "GJ_arrow_02_001.png"),
            this,
            menu_selector(JumpToPageLayer::onLast)
        );
        m_lastBtn->setPosition({ 50, -50});
        m_lastBtn->setID("last-button"_spr);
        m_buttonMenu->addChild(m_lastBtn);

        if(m_infoLayer->m_pageEndIdx == 0) {
            m_lastBtn->setVisible(false);
            m_infoLayer->loadPage(0, false);
            // ugly delegate swap because it's the easiest way to maintain the params
            GameLevelManager::sharedState()->m_levelCommentDelegate = this;
        }
    }

    return true;
}

CCLabelBMFont* JumpToPageLayer::createTextLabel(const std::string text, const CCPoint& position, const float scale, CCNode* menu, const char* font){
    CCLabelBMFont* bmFont = CCLabelBMFont::create(text.c_str(), font);
    bmFont->setPosition(position);
    bmFont->setScale(scale);
    menu->addChild(bmFont);
    return bmFont;
}