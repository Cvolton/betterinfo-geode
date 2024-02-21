#include "CreatorInfoPopup.h"

#include "../utils.hpp"

CreatorInfoPopup* CreatorInfoPopup::create(int userID){
    auto ret = new CreatorInfoPopup();
    if (ret && ret->init(userID)) {
        //robert 1 :D
        ret->autorelease();
    } else {
        //robert -1
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool CreatorInfoPopup::init(int userID){
    if(!CvoltonAlertLayerStub::init({380.0f, 210.0f}, 0.8f, 0x4B, 2)) return false;
    m_closeBtn->setVisible(false);

    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto GLM = GameLevelManager::sharedState();

    m_searchObject = GJSearchObject::create(SearchType::UsersLevels, std::to_string(userID));
    m_searchObject->m_starFilter = true;
    m_searchObject->retain();

    m_levels = CCArray::create();
    m_levels->retain();
    //TODO: release

    m_diffMenu = CCMenu::create();
    m_diffMenu->setLayout(
        RowLayout::create()
            ->setGap(16.f)
    );
    m_diffMenu->setContentSize({370, 50});
    m_diffMenu->setPosition({winSize.width / 2, winSize.height / 2 + 30});
    m_diffMenu->setID("diff-menu"_spr);
    m_mainLayer->addChild(m_diffMenu);

    m_secondRowMenu = CCMenu::create();
    m_secondRowMenu->setLayout(m_diffMenu->getLayout());
    m_secondRowMenu->setContentSize(m_diffMenu->getContentSize());
    m_secondRowMenu->setPosition({winSize.width / 2, winSize.height / 2 - 10});
    m_secondRowMenu->setID("second-row-menu"_spr);
    m_mainLayer->addChild(m_secondRowMenu);

    m_thirdRowMenu = CCMenu::create();
    m_thirdRowMenu->setLayout(m_diffMenu->getLayout());
    m_thirdRowMenu->setContentSize(m_diffMenu->getContentSize());
    m_thirdRowMenu->setPosition({winSize.width / 2, winSize.height / 2 - 35});
    m_thirdRowMenu->setID("third-row-menu"_spr);
    m_mainLayer->addChild(m_thirdRowMenu);

    m_cornerMenu = CCMenu::create();
    m_cornerMenu->setLayout(ColumnLayout::create()
        ->setGap(14.f)
        ->setAxisAlignment(AxisAlignment::Start)
        ->setCrossAxisLineAlignment(AxisAlignment::End)
        ->setAxisReverse(true)
    );
    m_cornerMenu->setContentSize({100, 182});
    m_cornerMenu->setPosition({winSize.width / 2 + 178, winSize.height / 2});
    m_cornerMenu->setAnchorPoint({1, .5f});
    m_cornerMenu->setID("corner-menu"_spr);
    m_mainLayer->addChild(m_cornerMenu);

    m_circle = LoadingCircle::create();
    m_circle->setParent(m_mainLayer);
    m_circle->show();

    loadLevels();

    return true;
}

void CreatorInfoPopup::showResults() {
    m_diffMenu->removeAllChildren();
    m_secondRowMenu->removeAllChildren();
    m_thirdRowMenu->removeAllChildren();

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    for(int i = -1; i <= 6; i++) {
        if(i == 0) continue;

        auto diff = GJDifficultySprite::create(i, (GJDifficultyName) 0);
        diff->setID(fmt::format("{}-diff", i));
        m_diffMenu->addChild(diff);

        auto bmFont1 = CCLabelBMFont::create(std::to_string(levelsForDifficulty(i, false)).c_str(), "goldFont.fnt");
        bmFont1->setScale(.6f);

        auto bmFont1Container = CCNode::create();
        bmFont1Container->addChild(bmFont1);
        bmFont1Container->setContentSize(diff->getContentSize());
        bmFont1Container->setID(fmt::format("{}-text1", i));

        bmFont1->setPosition(bmFont1Container->getContentSize() / 2);
        m_secondRowMenu->addChild(bmFont1Container);

        auto bmFont2 = CCLabelBMFont::create(std::to_string(levelsForDifficulty(i, true)).c_str(), "goldFont.fnt");
        bmFont2->setScale(.6f);
        bmFont2->setColor({255, 200, 255});

        auto bmFont2Container = CCNode::create();
        bmFont2Container->addChild(bmFont2);
        bmFont2Container->setContentSize(diff->getContentSize());
        bmFont2Container->setID(fmt::format("{}-text2", i));

        bmFont2->setPosition(bmFont2Container->getContentSize() / 2);
        m_thirdRowMenu->addChild(bmFont2Container);
    }

    m_diffMenu->updateLayout();
    m_secondRowMenu->updateLayout();
    m_thirdRowMenu->updateLayout();

    auto bmFont1 = CCLabelBMFont::create(fmt::format("Classic: {}", 69).c_str(), "goldFont.fnt");
    bmFont1->setScale(.7f);
    bmFont1->setPosition({winSize.width / 2 - 80, winSize.height / 2 + 85});
    bmFont1->setID("classic-text");
    m_mainLayer->addChild(bmFont1);

    auto bmFont2 = CCLabelBMFont::create(fmt::format("Platformer: {}", 69).c_str(), "goldFont.fnt");
    bmFont2->setScale(.7f);
    bmFont2->setPosition({winSize.width / 2 + 60, winSize.height / 2 + 85});
    bmFont2->setColor({255, 200, 255});
    bmFont2->setID("platformer-text");
    m_mainLayer->addChild(bmFont2);

    auto buttonSprite = ButtonSprite::create("OK", 0, false, "goldFont.fnt", "GJ_button_01.png", 0, 1.f);
    auto buttonButton = CCMenuItemSpriteExtra::create(
        buttonSprite,
        this,
        menu_selector(CreatorInfoPopup::onClose)
    );
    buttonButton->setSizeMult(1.2f);
    buttonButton->setPosition({0, -80});
    buttonButton->setID("ok-button"_spr);
    m_buttonMenu->addChild(buttonButton);

    /*auto cornerFont1 = CCLabelBMFont::create(fmt::format("Daily: {}", BetterInfo::completedLevelsInStarRange(0, 10, false, GLM->m_dailyLevels).size() + BetterInfo::completedLevelsInStarRange(0, 10, true, GLM->m_dailyLevels).size()).c_str(), "goldFont.fnt");
    cornerFont1->setLayoutOptions(AxisLayoutOptions::create()
        ->setMinScale(.0f)
        ->setMaxScale(.4f)
    );
    cornerFont1->setAnchorPoint({1, 0});
    cornerFont1->setID("daily-text"_spr);
    m_cornerMenu->addChild(cornerFont1);

    auto cornerFont2 = CCLabelBMFont::create(fmt::format("Gauntlet: {}", BetterInfo::completedLevelsInStarRange(0, 10, false, GLM->m_gauntletLevels).size() + BetterInfo::completedLevelsInStarRange(0, 10, true, GLM->m_gauntletLevels).size()).c_str(), "goldFont.fnt");
    cornerFont2->setLayoutOptions(BetterInfo::copyLayoutOptions(cornerFont1));
    cornerFont2->setAnchorPoint(cornerFont1->getAnchorPoint());
    cornerFont2->setID("gauntlet-text"_spr);
    m_cornerMenu->addChild(cornerFont2);*/

    m_cornerMenu->updateLayout();

    m_circle->fadeAndRemove();
    m_circle = nullptr;
}

void CreatorInfoPopup::loadLevels() {
    log::info("page {}", m_searchObject->m_page);

    auto GLM = GameLevelManager::sharedState();
    GLM->getOnlineLevels(m_searchObject);
    GLM->m_levelManagerDelegate = this;
}

void CreatorInfoPopup::loadLevelsFinished(cocos2d::CCArray* levels, const char*) {
    for(auto level : CCArrayExt<GJGameLevel*>(levels)) {
        m_levels->addObject(level);
    }

    m_searchObject->m_page++;
    loadLevels();
}

void CreatorInfoPopup::loadLevelsFailed(const char*) {
    showResults();
}

int CreatorInfoPopup::levelsForDifficulty(int difficulty, bool platformer){
    int min, max, total = 0;
    switch(difficulty) {
        case -1:
            min = max = 1;
            break;
        case 0:
            min = max = 0;
            break;
        case 1:
            min = max = 2;
            break;
        case 2:
            min = max = 3;
            break;
        case 3: 
            min = 4;
            max = 5;
            break;
        case 4:
            min = 6;
            max = 7;
            break;
        case 5:
            min = 8;
            max = 9;
            break;
        case 6:
            min = 10;
            max = 10;
            break;
    }

    for(auto level : CCArrayExt<GJGameLevel*>(m_levels)) {
        if(level->m_stars < min || level->m_stars > max) continue;
        if(platformer != level->isPlatformer()) continue;
        total += 1;
    }

    return total;
}