#include "RewardCategoryCell.h"
#include "RewardCell.h"
#include "RewardViewLayer.h"
#include "Geode/binding/ButtonSprite.hpp"

void RewardCategoryCell::onView(CCObject* sender) {
    auto browserLayer = RewardViewLayer::scene(m_category->filteredChests(), m_category->m_title.c_str());
    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);
    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void RewardCategoryCell::loadFromData(CCObject* object) {
    m_category = static_cast<RewardCategoryObject*>(object);

    const float rowX = 48.f;
    const float rowY = 10.5f;

    CCSprite* chest = CCSprite::createWithSpriteFrameName(m_category->m_icon.c_str());
    chest->setPosition({25, 22});
    chest->setScale((120.5f / chest->getContentWidth()) * 0.275f);
    this->m_mainLayer->addChild(chest);

    auto title = CCLabelBMFont::create(fmt::format("{} Chests", m_category->m_title).c_str(), "bigFont.fnt");
    title->setAnchorPoint({ 0.0f, .5f });
    title->setPosition(rowX, 31.5f);
    title->limitLabelWidth(356-rowX, .65f, .4f);
    this->m_mainLayer->addChild(title);
    
    auto menu = CCMenu::create();
    menu->setPosition({0, 0});
    this->m_mainLayer->addChild(menu);

    auto viewButton = ButtonSprite::create("View", 0, 0, "bigFont.fnt", "GJ_button_01.png", 32, .6f);
    viewButton->setScale(.9f);
    auto viewBtn = CCMenuItemSpriteExtra::create(viewButton, this, menu_selector(RewardCategoryCell::onView));
    viewBtn->setPosition({m_width - viewBtn->getScaledContentSize().width + 18, m_height / 2});
    menu->addChild(viewBtn);

    CCSprite* lastSprite = nullptr;
    CCLabelBMFont* lastText = nullptr;
    for(auto [type, count] : m_category->countSpecialItems()) {
        // this could likely be simplified by using geode layouts
        // but that's a problem for future me
        lastSprite = RewardCell::itemToSprite(type);
        if(type == SpecialRewardItem::CustomItem) {
            lastSprite = GJItemIcon::createBrowserItem(UnlockType::Cube, 1);
            lastSprite->setScale(.45f);
        }
        if(lastText == nullptr) lastSprite->setPosition({rowX + 1, rowY});
        else lastSprite->setPosition({lastText->getPositionX() + (lastText->getContentSize().width * lastText->getScaleX()) + 11.f, rowY});
        lastSprite->setAnchorPoint({0, 0.5f});
        this->m_mainLayer->addChild(lastSprite);

        lastText = CCLabelBMFont::create(fmt::format(std::locale("en_US.UTF-8"), "{:L}", count).c_str(), "bigFont.fnt");
        lastText->setAnchorPoint({ 0.0f, 0.5f });
        lastText->setPosition({lastSprite->getPositionX() + ((lastSprite->getContentSize().width) * lastSprite->getScaleX()) + 5, rowY});
        lastText->setScale(.325f);
        this->m_mainLayer->addChild(lastText);
    }

    this->m_mainLayer->setVisible(true);
    this->m_backgroundLayer->setOpacity(255);
}

RewardCategoryCell::RewardCategoryCell(const char* name, CCSize size) :
    GenericListCell(name, size) {}

RewardCategoryCell* RewardCategoryCell::create(const char* key, CCSize size) {
    auto ret = new RewardCategoryCell(key, size);
    if (ret && ret->init()) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}