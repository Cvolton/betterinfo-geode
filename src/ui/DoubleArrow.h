#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class DoubleArrow : public CCNode {
    CCSprite* m_firstArrow = nullptr;
    CCSprite* m_secondArrow = nullptr;
    bool m_flipped = false;
protected:
    bool init(bool flipped, const char* texture = "GJ_arrow_02_001.png");
    static const char* defaultTextureName(bool usePopupTexture);
public:
    static DoubleArrow* create(bool flipped, const char* texture = "GJ_arrow_02_001.png");
    static DoubleArrow* create(bool flipped, bool usePopupTexture);
    void changeTexture(const char* texture);
    void usePopupTexture(bool shouldUse);
};