#include <Geode/Geode.hpp>
#include <Geode/modify/StarInfoPopup.hpp>

#include "../utils.hpp"

using namespace geode::prelude;

class BI_DLL $modify(BIStarInfoPopup, StarInfoPopup) {
	bool init(int a, int b, int c, int d, int e, int f, int gauntlet, int daily, int unknown, bool platformer) {
		if (!StarInfoPopup::init(a,b,c,d,e,f,gauntlet,daily,unknown,platformer)) return false;

		if (platformer) return true;

		int eventLevels = BetterInfo::countCompletedEventLevels(false);
		if (eventLevels < 0) return true;

		auto eventLevelsLabel = BetterInfo::createEventLevelsLabel(eventLevels);
		if (!eventLevelsLabel) return true;

		auto menu = m_mainLayer->getChildByID("bottom-right-menu");
		if (!menu) return true;

		eventLevelsLabel->setID("event-text"_spr);
		menu->addChild(eventLevelsLabel);
		menu->updateLayout();

		return true;
	}
};