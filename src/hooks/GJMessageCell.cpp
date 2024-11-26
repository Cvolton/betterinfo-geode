#include <Geode/Geode.hpp>
#include <Geode/modify/GJMessageCell.hpp>

#include "../utils.hpp"

using namespace geode::prelude;

class BI_DLL $modify(BIGJMessageCell, GJMessageCell) {
    void loadFromMessage(GJUserMessage* b) {
        GJMessageCell::loadFromMessage(b);

        if(m_message->m_read || !m_message->m_outgoing) return;

        if(auto title = m_mainLayer->getChildByID("title-label")) {
            auto marker = cocos2d::CCLabelBMFont::create("!", "bigFont.fnt");
            marker->setScale(0.5f);
            marker->setAnchorPoint({0.0f, 0.5f});
            marker->setPosition(title->getPosition() + CCPoint(title->getScaledContentWidth() + 5.0f, 0.0f));
            marker->setColor({0, 255, 0});
            marker->setID("unread-mark"); // no _spr because we're emulating a vanilla node
            m_mainLayer->addChild(marker);
        }
    }
};