#include <Geode/Geode.hpp>
#include <Geode/modify/DemonInfoPopup.hpp>
#include <Geode/modify/ProfilePage.hpp>

#include "../utils.hpp"

using namespace geode::prelude;

static int lastStatInfoID = 0;

class BI_DLL $modify(ProfilePage) {
    void onStatInfo(CCObject* obj) {
        lastStatInfoID = m_accountID;
        ProfilePage::onStatInfo(obj);
        lastStatInfoID = 0;
    }
};

class BI_DLL $modify(BIDemonInfoPopup, DemonInfoPopup) {
	bool init(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int weekly, int gauntlet) {
		if (!DemonInfoPopup::init(a,b,c,d,e,f,g,h,i,j,weekly,gauntlet)) return false;

        if(auto userInfo = typeinfo_cast<GJUserScore*>(GameLevelManager::sharedState()->m_storedUserInfo->objectForKey(lastStatInfoID))) {
            auto sum = a + b + c + d + e + f + g + h + i + j + weekly + gauntlet;
            if(userInfo->m_demons <= sum) return true;

            if(auto menu = m_mainLayer->getChildByID("bottom-left-menu")) {
                auto text = CCLabelBMFont::create(
                    fmt::format("Derated: {}", userInfo->m_demons - sum).c_str(),
                    "goldFont.fnt"
                );

                menu->addChild(text);
                menu->updateLayout();
            }
        }

		return true;
	}
};