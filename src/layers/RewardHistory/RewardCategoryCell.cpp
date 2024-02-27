#include "RewardCategoryCell.h"
#include "RewardViewLayer.h"
#include "Geode/binding/ButtonSprite.hpp"

void RewardCategoryCell::onView(CCObject* sender) {
    auto browserLayer = RewardViewLayer::scene(m_category->filteredChests(), m_category->m_title.c_str());
    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);
    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void RewardCategoryCell::loadFromData(CCObject* object) {
    m_category = static_cast<RewardCategoryObject*>(object);
    m_category->retain();

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

    auto viewButton = ButtonSprite::create("View", 0, 0, "bigFont.fnt", "GJ_button_04.png", 32, .6f);
    auto viewBtn = CCMenuItemSpriteExtra::create(viewButton, this, menu_selector(RewardCategoryCell::onView));
    viewBtn->setPosition({m_width - viewBtn->getScaledContentSize().width + 20, m_height / 2});
    menu->addChild(viewBtn);


    /*CCObject* obj;
    CCSprite* lastSprite = nullptr;
    CCLabelBMFont* lastText = nullptr;
    CCARRAY_FOREACH(m_rewardItem->m_rewardObjects, obj){
        auto rewardObj = static_cast<GJRewardObject*>(obj);

        const char* textureName = "GJ_sRecentIcon_001.png";
        float scale = 1;
        switch(rewardObj->m_specialRewardItem) {
            case SpecialRewardItem::FireShard: textureName = "fireShardSmall_001.png"; break;
            case SpecialRewardItem::IceShard: textureName = "iceShardSmall_001.png"; break;
            case SpecialRewardItem::PoisonShard: textureName = "poisonShardSmall_001.png"; break;
            case SpecialRewardItem::ShadowShard: textureName = "shadowShardSmall_001.png"; break;
            case SpecialRewardItem::LavaShard: textureName = "lavaShardSmall_001.png"; break;
            case SpecialRewardItem::BonusKey: textureName = "GJ_bigKey_001.png"; scale = .5f; break;
            case SpecialRewardItem::Orbs: textureName = "currencyOrbIcon_001.png"; scale = .7f; break;
            case SpecialRewardItem::Diamonds: textureName = "diamond_small01_001.png"; break;
            case SpecialRewardItem::CustomItem: textureName = "collaborationIcon_001.png"; scale = .5f; break;
            case SpecialRewardItem::EarthShard: textureName = "shard0201ShardSmall_001.png"; break;
            case SpecialRewardItem::BloodShard: textureName = "shard0202ShardSmall_001.png"; break;
            case SpecialRewardItem::MetalShard: textureName = "shard0203ShardSmall_001.png"; break;
            case SpecialRewardItem::LightShard: textureName = "shard0204ShardSmall_001.png"; break;
            case SpecialRewardItem::SoulShard: textureName = "shard0205ShardSmall_001.png"; break;
            default: scale = 0.7f;
        }

        lastSprite = CCSprite::createWithSpriteFrameName(textureName);
        if(rewardObj->m_specialRewardItem == SpecialRewardItem::CustomItem) {
            lastSprite = GJItemIcon::createBrowserItem(rewardObj->m_unlockType, rewardObj->m_itemID);
        }
        if(lastText == nullptr) lastSprite->setPosition({rowX + 1, rowY});
        else lastSprite->setPosition({lastText->getPositionX() + (lastText->getContentSize().width * lastText->getScaleX()) + 11.f, rowY});
        lastSprite->setAnchorPoint({0, 0.5f});
        lastSprite->setScale(scale);
        this->m_mainLayer->addChild(lastSprite);

        lastText = CCLabelBMFont::create(CCString::createWithFormat("%i", rewardObj->m_total)->getCString(), "bigFont.fnt");
        lastText->setAnchorPoint({ 0.0f, 0.5f });
        lastText->setPosition({lastSprite->getPositionX() + ((lastSprite->getContentSize().width) * lastSprite->getScaleX()) + 5, rowY});
        lastText->setScale(.325f);
        this->m_mainLayer->addChild(lastText);
    }*/

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

RewardCategoryCell::~RewardCategoryCell() {
    m_category->release();
}