#include <Geode/Geode.hpp>
#include <Geode/modify/GJUserCell.hpp>

#include "../utils.hpp"

using namespace geode::prelude;

class BI_DLL $modify(BIGJUserCell, GJUserCell) {
    void loadFromScore(GJUserScore* b) {
        GJUserCell::loadFromScore(b);

        if(auto timestamp = typeinfo_cast<CCInteger*>(b->getUserObject("key_64"_spr))) {
            auto label = Label::create(
                fmt::format("Friends since: {}", TimeUtils::timeToString(timestamp->getValue(), true)),
                "chatFont.fnt"
            );
            label->setScale(.35f);
            label->setAnchorPoint({1, 0});
            label->setPosition({330, 2});
            label->setColor({51,51,51});
            label->setOpacity(200);
            label->setID("friends-since-label"_spr);
            m_mainLayer->addChild(label);
        }
    }
};