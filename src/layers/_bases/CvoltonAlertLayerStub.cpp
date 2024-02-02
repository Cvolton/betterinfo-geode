#include "CvoltonAlertLayerStub.h"

bool CvoltonAlertLayerStub::init(CCPoint contentSize, float closeBtnScale, uint8_t alpha, int squareNumber){
	bool init = FLAlertLayer::init(alpha);
    if(!init) return false;

    m_alertSize = contentSize;

	auto winSize = CCDirector::sharedDirector()->getWinSize();
	auto bg = CCScale9Sprite::create(fmt::format("GJ_square{:02}.png", squareNumber).c_str(), { 0.0f, 0.0f, 80.0f, 80.0f });
    bg->setContentSize(m_alertSize);
    bg->setPosition({ winSize.width / 2, winSize.height / 2 });
    bg->setID("background"_spr);
    m_mainLayer->addChild(bg, -1);

    m_buttonMenu = CCMenu::create();
    m_buttonMenu->setID("button-menu"_spr);
    m_mainLayer->addChild(m_buttonMenu, 10);

    m_closeBtn = createButton("GJ_closeBtn_001.png", {-((m_alertSize.x)/2) + 9.5f, (m_alertSize.y / 2) - 10}, menu_selector(CvoltonAlertLayerStub::onCloseThunk), closeBtnScale);
    m_closeBtn->setID("close-button"_spr);

    return true;
}

void CvoltonAlertLayerStub::createTitle(std::string text, float separatorScale, float usernameScale){
	auto userName = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
    userName->setPosition({0, (m_alertSize.y/2) - 22});
    userName->setScale(usernameScale);
    userName->setID("title-label"_spr);
    m_buttonMenu->addChild(userName);

    auto separator = CCSprite::createWithSpriteFrameName("floorLine_001.png");
    separator->setPosition({0, (m_alertSize.y/2) - 46});
    separator->setScaleX(separatorScale);
    separator->setOpacity(100);
    separator->setID("separator"_spr);
    m_buttonMenu->addChild(separator);
}

CCMenuItemSpriteExtra* CvoltonAlertLayerStub::createButton(const char* texture, CCPoint position, SEL_MenuHandler callback, float textureScale, float sizeMult){
	auto buttonSprite = CCSprite::createWithSpriteFrameName(texture);
    buttonSprite->setScale(textureScale);
	auto button = CCMenuItemSpriteExtra::create(
        buttonSprite,
        this,
        callback
    );
    m_buttonMenu->addChild(button);
    button->setPosition(position);
    button->setSizeMult(sizeMult);
    return button;
}

void CvoltonAlertLayerStub::keyBackClicked() {
    onClose(nullptr);
}

void CvoltonAlertLayerStub::onCloseThunk(CCObject* sender) {
    onClose(sender);
}

void CvoltonAlertLayerStub::onClose(CCObject* sender) {
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void CvoltonAlertLayerStub::registerWithTouchDispatcher() {
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -250, true);
}