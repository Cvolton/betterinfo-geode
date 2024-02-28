#include "CreatorInfoPopup.h"

#include "../utils.hpp"
#include <string>

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
    //m_searchObject->m_starFilter = true;
    m_searchObject->retain();

    m_levels = CCArray::create();
    m_levels->retain();

    m_tabMenu = CCMenu::create();
    m_tabMenu->setLayout(
        RowLayout::create()
            ->setGap(16.f)
    );
    m_tabMenu->setContentSize({366, 50});
    m_tabMenu->setPosition({winSize.width / 2, winSize.height / 2 + 113});
    m_tabMenu->setID("tab-menu"_spr);
    m_tabMenu->setZOrder(-2);
    m_mainLayer->addChild(m_tabMenu);

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
    m_circle->setParentLayer(m_mainLayer);
    m_circle->setPosition({0, 11});
    m_circle->show();

    m_loading = CCLabelBMFont::create("0%", "goldFont.fnt");
    m_loading->setScale(.6f);
    m_loading->setPosition({winSize.width / 2, winSize.height / 2 + 12});
    m_loading->setID("loading-text"_spr);
    m_loading->setVisible(false);
    m_mainLayer->addChild(m_loading);
    
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

    loadLevels();
    createTabs();

    return true;
}

void CreatorInfoPopup::onClose(cocos2d::CCObject* sender)
{
    if(m_circle) m_circle->fadeAndRemove();
    if(m_searchObject) m_searchObject->release();

    auto GLM = GameLevelManager::sharedState();
    GLM->m_levelManagerDelegate = nullptr;

    CvoltonAlertLayerStub::onClose(sender);
}

void CreatorInfoPopup::onTab(cocos2d::CCObject* sender) {
    auto button = typeinfo_cast<CCMenuItemSpriteExtra*>(sender);
    if(!button) return;

    m_tab = button->getTag();
    createTabs();
    if(m_loaded) showResults();
}

CCMenuItemSpriteExtra* CreatorInfoPopup::createTab(const char* text, const char* icon, int tab, bool current) {
    if(tab == m_tab && !current) {
        m_currentTab = createTab(text, icon, tab, true);
        m_buttonMenu->addChild(m_currentTab);
    }

    auto sprite = BetterInfo::createBISprite(tab == m_tab ? "BI_blueTab_002.png" : "BI_blueTab_001.png");
    auto font = CCLabelBMFont::create(text, "bigFont.fnt");
    sprite->addChild(font);
    font->setPosition(sprite->getContentSize() / 2);
    font->limitLabelWidth(sprite->getContentSize().width - 15, .65f, .1f);
    //auto button = BetterInfo::createSearchButton(this, text, icon, nullptr, .5f, .75f);
    //auto button = TabButton::create(text, this, nullptr);
    auto button = CCMenuItemSpriteExtra::create(
        sprite,
        this,
        menu_selector(CreatorInfoPopup::onTab)
    );
    button->setTag(tab);
    button->m_colorDip = .8f;
    button->m_colorEnabled = true;
    button->m_scaleMultiplier = 1.f;
    button->setID(Mod::get()->expandSpriteName(fmt::format("tab-{}", tab).c_str()));
    button->setEnabled(tab != m_tab);
    return button;
}

void CreatorInfoPopup::createTabs() {
    m_tabMenu->removeAllChildren();

    if(m_currentTab) {
        m_currentTab->removeFromParent();
        m_currentTab = nullptr;
    }

    m_tabMenu->addChild(createTab("Total", nullptr, 0));
    m_tabMenu->addChild(createTab("Rated", nullptr, 1));
    m_tabMenu->addChild(createTab("Featured", nullptr, 2));
    m_tabMenu->addChild(createTab("Epic", nullptr, 3));
    m_tabMenu->addChild(createTab("Legendary", nullptr, 4));
    m_tabMenu->addChild(createTab("Mythic", nullptr, 5));

    m_tabMenu->updateLayout();

    for(auto button : CCArrayExt<CCMenuItemSpriteExtra*>(m_tabMenu->getChildren())) {
        if(button->getTag() == m_tab) {
            m_currentTab->setPosition(m_buttonMenu->convertToNodeSpace(button->getParent()->convertToWorldSpace(button->getPosition())));
            m_currentTab->setScale(button->getScale());
        }
    }
}

void CreatorInfoPopup::showResults() {
    m_loading->setVisible(false);
    m_diffMenu->removeAllChildren();
    m_secondRowMenu->removeAllChildren();
    m_thirdRowMenu->removeAllChildren();
    if(m_classic) m_classic->removeFromParent();
    if(m_platformer) m_platformer->removeFromParent();

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

    m_classic = CCLabelBMFont::create(fmt::format("Classic: {}", levelsForDifficulty(-10, false)).c_str(), "goldFont.fnt");
    m_classic->setScale(.7f);
    m_classic->setPosition({winSize.width / 2 - 80, winSize.height / 2 + 82});
    m_classic->setID("classic-text");
    m_mainLayer->addChild(m_classic);

    m_platformer = CCLabelBMFont::create(fmt::format("Platformer: {}", levelsForDifficulty(-10, true)).c_str(), "goldFont.fnt");
    m_platformer->setScale(.7f);
    m_platformer->setPosition({winSize.width / 2 + 60, winSize.height / 2 + 82});
    m_platformer->setColor({255, 200, 255});
    m_platformer->setID("platformer-text");
    m_mainLayer->addChild(m_platformer);

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

    if(m_circle) {
        m_circle->fadeAndRemove();
        m_circle = nullptr;
    }

    m_loaded = true;
}

void CreatorInfoPopup::loadLevels() {
    log::debug("Calculating creator info: page {}", m_searchObject->m_page);

    auto GLM = GameLevelManager::sharedState();
    auto storedLevels = GLM->getStoredOnlineLevels(m_searchObject->getKey());
    if(storedLevels) {
        loadLevelsFinished(storedLevels, "");
    } else {
        //this->getScheduler()->scheduleSelector(schedule_selector(CreatorInfoPopup::queueLoad), this, 1, 0, 0.5f, false);
        CreatorInfoPopup::queueLoad(0);
    }
}

void CreatorInfoPopup::queueLoad(float dt) {
    auto GLM = GameLevelManager::sharedState();
    GLM->m_levelManagerDelegate = this;
    GLM->getOnlineLevels(m_searchObject);
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

void CreatorInfoPopup::setupPageInfo(gd::string counts, const char* key) {

    std::vector<int> countsVec;
    std::stringstream responseStream(counts);
    std::string currentKey;
    while(std::getline(responseStream, currentKey, ':')) {
        countsVec.push_back(BetterInfo::stoi(currentKey));
    }

    if(countsVec.size() != 3) return;
    if(countsVec[0] == 0) return;

    if(m_loading) m_loading->setString(fmt::format("{}%", 100 * countsVec[1] / countsVec[0]).c_str());
}

int CreatorInfoPopup::levelsForDifficulty(int difficulty, bool platformer){
    int total = 0;

    for(auto level : CCArrayExt<GJGameLevel*>(m_levels)) {
        auto levelDifficulty = level->m_ratings > 0 ? (level->m_ratingsSum / level->m_ratings) : 0;
        if(difficulty == -1 && !level->m_autoLevel) continue;
        if(difficulty > 0 && difficulty < 6 && (levelDifficulty != difficulty || level->m_autoLevel || level->m_demon)) continue;
        if(difficulty == 6 && !level->m_demon) continue;

        if(m_tab == 1 && level->m_stars <= 0) continue;
        if(m_tab == 2 && level->m_featured <= 0) continue;
        if(m_tab == 3 && level->m_isEpic <= 0) continue;
        if(m_tab == 4 && level->m_isEpic <= 1) continue;
        if(m_tab == 5 && level->m_isEpic <= 2) continue;

        if(platformer != level->isPlatformer()) continue;
        total += 1;
    }

    return total;
}

CreatorInfoPopup::~CreatorInfoPopup() {
    if(m_circle) m_circle->fadeAndRemove();
    if(m_searchObject) m_searchObject->release();
}