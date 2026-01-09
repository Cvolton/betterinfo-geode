#include <Geode/Geode.hpp>
#include <Geode/modify/GJMessagePopup.hpp>

using namespace geode::prelude;

class $modify(GJMessagePopup) {
    bool init(GJUserMessage* message) {
        if (!GJMessagePopup::init(message)) return false;

		// friend request id
		auto idText = CCLabelBMFont::create(fmt::format("#{}", message->m_messageID).c_str(), "chatFont.fnt");
		idText->setPosition({486, 260});
		idText->setAnchorPoint({1, .5f});
		idText->setScale(.35f);
		idText->setOpacity(200);
		idText->setColor({0,0,0});
		idText->setID("request-id"_spr);
		m_mainLayer->addChild(idText);

        return true;
    }
};