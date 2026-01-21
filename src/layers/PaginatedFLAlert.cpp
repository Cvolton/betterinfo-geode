#include "PaginatedFLAlert.h"

PaginatedFLAlert* PaginatedFLAlert::create(const std::string& title, const std::vector<std::string>& content, CCNode* parent, size_t page) {
    auto ret = new PaginatedFLAlert();
    if (ret && ret->init(title, content, parent, page)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool PaginatedFLAlert::init(const std::string& title, const std::vector<std::string>& content, CCNode* parent, size_t page) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    m_title = title;
    m_content = content;
    m_page = page;
    m_scene = parent;
    /*
        flalert
    */
    if(m_content.size() == 0) m_content.push_back("PaginatedFLAlert content is empty, this is very likely a coding mistake and should be reported as a bug to the mod author(s).");

    FLAlertLayer::init(nullptr, title.c_str(), content[page % m_content.size()], "OK", nullptr, 400, false, 300, 1);
    BetterInfo::fixOversizedPopup(this);

    /*
        next/prev page btn
    */
    auto y = -m_buttonMenu->getPosition().y + (winSize.height / 2);

    auto prevSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    prevSprite->setScale(.8f);
    auto prevBtn = CCMenuItemSpriteExtra::create(
        prevSprite,
        this,
        menu_selector(PaginatedFLAlert::onPrev)
    );
    prevBtn->setPosition({-220, y});
    m_buttonMenu->addChild(prevBtn);
    prevBtn->setVisible(m_page > 0);
    prevBtn->setID("prev-button"_spr);

    auto nextSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    nextSprite->setFlipX(true);
    nextSprite->setScale(.8f);
    auto nextBtn = CCMenuItemSpriteExtra::create(
        nextSprite,
        this,
        menu_selector(PaginatedFLAlert::onNext)
    );
    nextBtn->setPosition({220, y});
    nextBtn->setVisible(m_page < (m_content.size() - 1));
    nextBtn->setID("next-button"_spr);
    m_buttonMenu->addChild(nextBtn);

    return true;
}

void PaginatedFLAlert::onNext(cocos2d::CCObject* sender) {
    if(m_page+1 >= m_content.size()) return;
    loadPage(m_page+1);
}

void PaginatedFLAlert::onPrev(cocos2d::CCObject* sender) {
    if(m_page == 0) return;
    loadPage(m_page-1);
}

void PaginatedFLAlert::loadPage(size_t page) {
    auto title = m_title;
    auto content = m_content;
    auto parent = m_pParent ? m_pParent : CCDirector::sharedDirector()->getRunningScene();
    auto zOrder = getZOrder();

    onBtn1(this);

    auto newAlert = PaginatedFLAlert::create(title, content, parent, page);
    newAlert->setZOrder(zOrder);
    parent->addChild(newAlert);
}

void PaginatedFLAlert::keyDown(cocos2d::enumKeyCodes key, double timestamp) {
    switch(key) {
        case KEY_Left:
        case CONTROLLER_Left:
            onPrev(nullptr);
            break;
        case KEY_Right:
        case CONTROLLER_Right:
            onNext(nullptr);
            break;
        default:
            FLAlertLayer::keyDown(key, timestamp);
    }
}