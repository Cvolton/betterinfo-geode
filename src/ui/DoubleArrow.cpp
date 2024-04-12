#include "DoubleArrow.h"

bool DoubleArrow::init(bool flipped, const char* texture) {
    if(!CCNode::init()) return false;

    this->setContentSize({50,50});
    this->setScale(.4f);

    m_flipped = flipped;
    changeTexture(texture);

    return true;
}

const char* DoubleArrow::defaultTextureName(bool usePopupTexture) {
    return usePopupTexture ? "GJ_arrow_03_001.png" : "GJ_arrow_02_001.png";
}

DoubleArrow* DoubleArrow::create(bool flipped, const char* texture) {
    auto ret = new DoubleArrow();
    if(ret && ret->init(flipped, texture)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

DoubleArrow* DoubleArrow::create(bool flipped, bool usePopupTexture) {
    return DoubleArrow::create(flipped, defaultTextureName(usePopupTexture));
}

void DoubleArrow::changeTexture(const char* texture) {
    m_firstArrow = CCSprite::createWithSpriteFrameName(texture);
    m_firstArrow->setPosition({35,25});
    m_firstArrow->setFlipX(m_flipped);
    m_firstArrow->setID("first-arrow"_spr);
    m_secondArrow = CCSprite::createWithSpriteFrameName(texture);
    m_secondArrow->setPosition({15,25});
    m_secondArrow->setFlipX(m_flipped);
    m_secondArrow->setZOrder(m_flipped ? -1 : 1);
    m_secondArrow->setID("second-arrow"_spr);

    this->addChild(m_firstArrow);
    this->addChild(m_secondArrow);
}

void DoubleArrow::usePopupTexture(bool shouldUse) {
    changeTexture(defaultTextureName(shouldUse));
}