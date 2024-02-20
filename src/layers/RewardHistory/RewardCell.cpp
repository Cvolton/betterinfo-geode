#include "RewardCell.h"
#include "../../hooks/GJRewardItem.h"
#include "../../utils.hpp"
#include "../../managers/BetterInfoCache.h"

std::string RewardCell::getDisplayName() {
    auto key = static_cast<BIGJRewardItem*>(m_rewardItem)->getKey();
    if(key.empty()) return "Unknown";

    /**
     * Gauntlet chests
    */
    if(key[0] == 'g' && key[1] == '_') {
        key = key.substr(2);
        return fmt::format("{} Gauntlet", GauntletNode::nameForType((GauntletType) BetterInfo::stoi(key)));
    }

    /**
     * Path chests
    */
    if(key[0] == 'p' && key[1] == 'r' && key[2] == '_') {
        key = key.substr(3);
        return fmt::format("{} Path", GJPathsLayer::nameForPath(BetterInfo::stoi(key)));
    }

    /**
     * Other chests
    */
    std::array<const char*, 11> chestNames = {"Basement", "Demon Guardian", "Chamber of Time", "50 chests", "100 chests", "200 chests", "Facebook", "Twitter", "YouTube", "Twitch", "Discord"};

    size_t idx = 1;
    for(auto name : chestNames) {
        if(key == fmt::format("{:04}", idx, key)) return name;
        idx++;
    }

    for( ; idx <= 21; idx++) {
        if(key == fmt::format("{:04}", idx, key)) return "Ad Chest";
    }

    std::array<const char*, 3> chestNames2 = {"Zolguroth", "Help Screen", "Reddit"};
    for(auto name : chestNames2) {
        if(key == fmt::format("{:04}", idx, key)) return name;
        idx++;
    }

    /**
     * Daily Chests
    */
    if(key[0] == '1' && key[1] == '_') {
        key = key.substr(2);
        return fmt::format("Small Chest {}", key);
    }

    if(key[0] == '2' && key[1] == '_') {
        key = key.substr(2);
        return fmt::format("Large Chest {}", key);
    }

    /**
     * Weekly Chests
    */
    if(key[0] == 'd') {
        key = key.substr(1);

        auto level = typeinfo_cast<GJGameLevel*>(GameLevelManager::sharedState()->m_dailyLevels->objectForKey(key));
        if(level) return fmt::format("{}: {}", BetterInfo::stoi(key) - 100000, BetterInfoCache::sharedState()->getLevelName(level->m_levelID));

        return key;
    }

    /**
     * Treasure Room Chests
    */
    auto chestID = BetterInfo::stoi(key);
    if(chestID > 0 && std::to_string(chestID) == key) {
        if(chestID < 1000) return "1 Key Chest";
        if(chestID < 2000) return "5 Key Chest";
        if(chestID < 3000) return "10 Key Chest";
        if(chestID < 4000) return "25 Key Chest";
        if(chestID < 5000) return "50 Key Chest";
        if(chestID < 6000) return "100 Key Chest";
    }

    return key;
}

void RewardCell::loadFromData(CCObject* object) {
    m_rewardItem = static_cast<GJRewardItem*>(object);

    const float rowX = 48.f;
    const float rowY = 10.5f;

    const char* chestTexture = "chest_01_02_001.png";
    float chestSize = 0.35f;
    std::string chestTitle = Mod::get()->getSavedValue<std::string>("reward-cell-title");
    switch(m_rewardItem->m_rewardType) {
        default: break;
        case GJRewardType::Small: chestTexture = "chest_01_02_001.png"; break;
        case GJRewardType::Large: chestTexture = "chest_02_02_001.png"; chestSize = 0.275f; break;
        case GJRewardType::SmallTreasure: chestTexture = "chest_03_02_001.png"; chestSize = 0.275f; chestTitle = "1 Key"; break;
        case GJRewardType::LargeTreasure: chestTexture = "chest_04_02_001.png"; chestSize = 0.275f; chestTitle = "5 Key"; break;
        case GJRewardType::Key10Treasure: chestTexture = "chest_05_02_001.png"; chestSize = 0.275f; chestTitle = "10 Key"; break;
        case GJRewardType::Key25Treasure: chestTexture = "chest_06_02_001.png"; chestSize = 0.275f; chestTitle = "25 Key"; break;
        case GJRewardType::Key50Treasure: chestTexture = "chest_07_02_001.png"; chestSize = 0.275f; chestTitle = "50 Key"; break;
        case GJRewardType::Key100Treasure: chestTexture = "chest_08_02_001.png"; chestSize = 0.275f; chestTitle = "100 Key"; break;
    }
    CCSprite* chest = CCSprite::createWithSpriteFrameName(chestTexture);
    chest->setPosition({25, 22});
    chest->setScale(chestSize);
    this->m_mainLayer->addChild(chest);

    //auto title = CCLabelBMFont::create(CCString::createWithFormat(m_rewardItem->m_chestID != 0 ? "%s Chest %i" : "%s Chest", chestTitle.c_str(), m_rewardItem->m_chestID)->getCString(), "bigFont.fnt");
    auto title = CCLabelBMFont::create(getDisplayName().c_str(), "bigFont.fnt");
    title->setAnchorPoint({ 0.0f, .5f });
    title->setPosition(rowX, 31.5f);
    title->limitLabelWidth(356-rowX, .65f, .4f);
    this->m_mainLayer->addChild(title);


    CCObject* obj;
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
    }

    this->m_mainLayer->setVisible(true);
    this->m_backgroundLayer->setOpacity(255);
}

RewardCell::RewardCell(const char* name, CCSize size) :
    GenericListCell(name, size) {}

RewardCell* RewardCell::create(const char* key, CCSize size) {
    auto ret = new RewardCell(key, size);
    if (ret && ret->init()) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}