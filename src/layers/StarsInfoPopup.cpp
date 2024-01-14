#include "StarsInfoPopup.h"

#include "../utils.hpp"

StarsInfoPopup* StarsInfoPopup::create(){
    auto ret = new StarsInfoPopup();
    if (ret && ret->init()) {
        //robert 1 :D
        ret->autorelease();
    } else {
        //robert -1
        delete ret;
        ret = nullptr;
    }
    return ret;
}

int StarsInfoPopup::completedLevelsForDifficulty(int difficulty, bool platformer){
    int min, max = 0;
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
    }

    auto levels = BetterInfo::completedLevelsInStarRange(min, max, platformer);
    return levels.size();
}

bool StarsInfoPopup::init(){
    if(!createBasics({380.0f, 210.0f}, menu_selector(StarsInfoPopup::onClose), 0.8f, {0x00, 0x00, 0x00, 0x4B}, 2)) return false;
    m_closeBtn->setVisible(false);

    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto GLM = GameLevelManager::sharedState();

    auto diffMenu = CCMenu::create();
    diffMenu->setLayout(
        RowLayout::create()
            ->setGap(16.f)
    );
    diffMenu->setContentSize({370, 50});
    diffMenu->setPosition({winSize.width / 2, winSize.height / 2 + 30});
    m_mainLayer->addChild(diffMenu);

    auto secondRowMenu = CCMenu::create();
    secondRowMenu->setLayout(diffMenu->getLayout());
    secondRowMenu->setContentSize(diffMenu->getContentSize());
    secondRowMenu->setPosition({winSize.width / 2, winSize.height / 2 - 10});
    m_mainLayer->addChild(secondRowMenu);

    auto thirdRowMenu = CCMenu::create();
    thirdRowMenu->setLayout(diffMenu->getLayout());
    thirdRowMenu->setContentSize(diffMenu->getContentSize());
    thirdRowMenu->setPosition({winSize.width / 2, winSize.height / 2 - 35});
    m_mainLayer->addChild(thirdRowMenu);

    auto cornerMenu = CCMenu::create();
    cornerMenu->setLayout(ColumnLayout::create()
        ->setGap(14.f)
        ->setAxisAlignment(AxisAlignment::Start)
        ->setCrossAxisLineAlignment(AxisAlignment::End)
        ->setAxisReverse(true)
    );
    cornerMenu->setContentSize({100, 182});
    cornerMenu->setPosition({winSize.width / 2 + 178, winSize.height / 2});
    cornerMenu->setAnchorPoint({1, .5f});
    m_mainLayer->addChild(cornerMenu);

    for(int i = -1; i < 6; i++) {
        //if(i == 0) continue;

        auto diff = GJDifficultySprite::create(i, (GJDifficultyName) 0);
        diffMenu->addChild(diff);

        auto bmFont1 = CCLabelBMFont::create(std::to_string(completedLevelsForDifficulty(i, false)).c_str(), "goldFont.fnt");
        bmFont1->setScale(.6f);

        auto bmFont1Container = CCNode::create();
        bmFont1Container->addChild(bmFont1);
        bmFont1Container->setContentSize(diff->getContentSize());

        bmFont1->setPosition(bmFont1Container->getContentSize() / 2);
        secondRowMenu->addChild(bmFont1Container);

        auto bmFont2 = CCLabelBMFont::create(std::to_string(completedLevelsForDifficulty(i, true)).c_str(), "goldFont.fnt");
        bmFont2->setScale(.6f);
        bmFont2->setColor({255, 200, 255});

        auto bmFont2Container = CCNode::create();
        bmFont2Container->addChild(bmFont2);
        bmFont2Container->setContentSize(diff->getContentSize());

        bmFont2->setPosition(bmFont2Container->getContentSize() / 2);
        thirdRowMenu->addChild(bmFont2Container);
    }

    diffMenu->updateLayout();
    secondRowMenu->updateLayout();
    thirdRowMenu->updateLayout();

    auto bmFont1 = CCLabelBMFont::create(fmt::format("Classic: {}", BetterInfo::completedLevelsInStarRange(0, 10, false).size()).c_str(), "goldFont.fnt");
    bmFont1->setScale(.7f);
    bmFont1->setPosition({winSize.width / 2 - 80, winSize.height / 2 + 85});
    m_mainLayer->addChild(bmFont1);

    auto bmFont2 = CCLabelBMFont::create(fmt::format("Platformer: {}", BetterInfo::completedLevelsInStarRange(0, 10, true).size()).c_str(), "goldFont.fnt");
    bmFont2->setScale(.7f);
    bmFont2->setPosition({winSize.width / 2 + 60, winSize.height / 2 + 85});
    bmFont2->setColor({255, 200, 255});
    m_mainLayer->addChild(bmFont2);

    auto buttonSprite = ButtonSprite::create("OK", 0, false, "goldFont.fnt", "GJ_button_01.png", 0, 1.f);
    auto buttonButton = CCMenuItemSpriteExtra::create(
        buttonSprite,
        this,
        menu_selector(StarsInfoPopup::onClose)
    );
    buttonButton->setSizeMult(1.2f);
    buttonButton->setPosition({0, -80});
    m_buttonMenu->addChild(buttonButton);

    auto cornerFont1 = CCLabelBMFont::create(fmt::format("Daily: {}", BetterInfo::completedLevelsInStarRange(0, 10, false, GLM->m_dailyLevels).size() + BetterInfo::completedLevelsInStarRange(0, 10, true, GLM->m_dailyLevels).size()).c_str(), "goldFont.fnt");
    cornerFont1->setLayoutOptions(AxisLayoutOptions::create()
        ->setMinScale(.0f)
        ->setMaxScale(.4f)
    );
    cornerFont1->setAnchorPoint({1, 0});
    cornerMenu->addChild(cornerFont1);

    auto cornerFont2 = CCLabelBMFont::create(fmt::format("Gauntlet: {}", BetterInfo::completedLevelsInStarRange(0, 10, false, GLM->m_gauntletLevels).size() + BetterInfo::completedLevelsInStarRange(0, 10, true, GLM->m_gauntletLevels).size()).c_str(), "goldFont.fnt");
    cornerFont2->setLayoutOptions(cornerFont1->getLayoutOptions());
    cornerFont2->setAnchorPoint(cornerFont1->getAnchorPoint());
    cornerMenu->addChild(cornerFont2);

    cornerMenu->updateLayout();

    return true;
}

void StarsInfoPopup::onClose(CCObject* sender){
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}