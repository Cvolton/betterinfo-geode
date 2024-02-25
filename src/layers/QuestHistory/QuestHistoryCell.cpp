#include "QuestHistoryCell.h"
#include "QuestHistoryObject.h"
#include "../../utils.hpp"
#include "../../managers/BetterInfoCache.h"

void QuestHistoryCell::loadFromData(CCObject* object) {
    auto data = dynamic_cast<QuestHistoryObject*>(object);

    auto diamondIcon = CCSprite::createWithSpriteFrameName("GJ_bigDiamond_001.png");
    diamondIcon->setPosition({20.0f, this->m_height / 2});
    diamondIcon->setScale(0.55f);
    m_mainLayer->addChild(diamondIcon);

    auto diamondLabel = CCLabelBMFont::create(
        fmt::format("{} Diamonds", data->m_diamondCount).c_str(),
        "bigFont.fnt"
    );
    diamondLabel->setPosition({38.0f, (this->m_height / 2) + .5f});
    diamondLabel->setScale(0.6f);
    diamondLabel->setAnchorPoint({0, 0.5f});
    diamondLabel->limitLabelWidth(95.5f, 0.5f, 0.1f);
    m_mainLayer->addChild(diamondLabel);

    auto completesIcon = CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
    completesIcon->setPosition({this->m_width - 113.0f, this->m_height / 2});
    completesIcon->setScale(0.85f);
    m_mainLayer->addChild(completesIcon);

    auto questLabel = CCLabelBMFont::create(
        fmt::format("{} Quests", data->m_questCount).c_str(),
        "bigFont.fnt"
    );
    questLabel->setPosition({this->m_width - 7.0f, (this->m_height / 2) + .5f});
    questLabel->setAnchorPoint({1, 0.5f});
    questLabel->limitLabelWidth(85.5f, 0.6f, 0.1f);
    m_mainLayer->addChild(questLabel);

    /*auto questIcon = CCSprite::createWithSpriteFrameName("GJ_questIcon_001.png");
    questIcon->setPosition({this->m_width - 60.0f, this->m_height / 2});
    questIcon->setScale(0.55f);
    m_mainLayer->addChild(questIcon);*/

    this->m_mainLayer->setVisible(true);
    this->m_backgroundLayer->setOpacity(255);
}

void QuestHistoryCell::updateBGColor(int idx) {
    /*auto idxNew = idx % 5;
    switch(idxNew) {
        case 0:
            this->m_backgroundLayer->setColor({0x5B, 0xCE, 0xFA});
            break;
        case 1:
            this->m_backgroundLayer->setColor({0xF5, 0xA9, 0xB8});
            break;
        case 2:
            this->m_backgroundLayer->setColor({0xFF, 0xFF, 0xFF});
            break;
        case 3:
            this->m_backgroundLayer->setColor({0xF5, 0xA9, 0xB8});
            break;
        case 4:
            this->m_backgroundLayer->setColor({0x5B, 0xCE, 0xFA});
            break;
    }*/

    /*if (idx % 2 == 1) {
        this->m_backgroundLayer->setColor({75, 75, 75});
    } else {
        this->m_backgroundLayer->setColor({50, 50, 50});
    }*/

    if (idx % 2 == 1) {
        //this->m_backgroundLayer->setColor({63, 60, 158});
        //this->m_backgroundLayer->setColor({48, 64, 130});
        //this->m_backgroundLayer->setColor({56, 59, 151});
        this->m_backgroundLayer->setColor({63, 63, 127});
    } else {
        this->m_backgroundLayer->setColor({50, 50, 100});
    }
}

QuestHistoryCell::QuestHistoryCell(const char* name, CCSize size) :
    GenericListCell(name, size) {}

QuestHistoryCell* QuestHistoryCell::create(const char* key, CCSize size) {
    auto ret = new QuestHistoryCell(key, size);
    if (ret && ret->init()) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}