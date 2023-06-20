#include "CvoltonAlertLayerStub.h"

bool CvoltonAlertLayerStub::createBasics(CCPoint contentSize, SEL_MenuHandler onClose, float closeBtnScale, const ccColor4B &color){
	bool init = CCLayerColor::initWithColor(color);
    if(!init) return false;

    m_alertSize = contentSize;

    CCDirector* director = CCDirector::sharedDirector();
    director->getTouchDispatcher()->incrementForcePrio(2);

    setTouchEnabled(true);
    setKeypadEnabled(true);

    m_mainLayer = CCLayer::create();

    this->addChild(m_mainLayer);

	CCSize winSize = director->getWinSize();
	extension::CCScale9Sprite* bg = extension::CCScale9Sprite::create("GJ_square01.png", { 0.0f, 0.0f, 80.0f, 80.0f });
    bg->setContentSize(m_alertSize);
    m_mainLayer->addChild(bg, -1);
    bg->setPosition({ winSize.width / 2, winSize.height / 2 });

    m_buttonMenu = CCMenu::create();
    m_buttonMenu->setID("button-menu");
    m_mainLayer->addChild(m_buttonMenu, 10);

    m_closeBtn = createButton("GJ_closeBtn_001.png", {-((m_alertSize.x)/2) + 9.5f, (m_alertSize.y / 2) - 10}, onClose, closeBtnScale);
    m_closeBtn->setID("close-button");

    return true;
}

void CvoltonAlertLayerStub::createTitle(std::string text, float separatorScale, float usernameScale){
	auto userName = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
    userName->setPosition({0, (m_alertSize.y/2) - 22});
    userName->setScale(usernameScale);
    m_buttonMenu->addChild(userName);

    auto separator = CCSprite::createWithSpriteFrameName("floorLine_001.png");
    separator->setPosition({0, (m_alertSize.y/2) - 46});
    separator->setScaleX(separatorScale);
    separator->setOpacity(100);
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