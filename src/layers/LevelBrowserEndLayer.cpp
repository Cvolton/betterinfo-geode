#include "LevelBrowserEndLayer.h"

LevelBrowserEndLayer* LevelBrowserEndLayer::create(LevelBrowserLayer* levelBrowserLayer){
    auto ret = new LevelBrowserEndLayer();
    if (ret && ret->init(levelBrowserLayer)) {
        //robert 1 :D
        ret->autorelease();
    } else {
        //robert -1
        delete ret;
        ret = nullptr;
    }
    return ret;
}

void LevelBrowserEndLayer::onClose(cocos2d::CCObject* sender)
{
    m_levelBrowserLayer->m_searchObject->m_page = m_min;
    if(m_levelBrowserLayer != nullptr) m_levelBrowserLayer->loadPage(m_levelBrowserLayer->m_searchObject);
    if(m_levelBrowserLayer != nullptr) m_levelBrowserLayer->release();
    if(m_circle != nullptr) m_circle->fadeAndRemove();

    auto GLM = GameLevelManager::sharedState();
    GLM->m_levelManagerDelegate = nullptr;

    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void LevelBrowserEndLayer::onOK(cocos2d::CCObject* sender){
    m_levelBrowserLayer->m_searchObject->m_page = 1;
    
    getOnlineLevels(m_levelBrowserLayer->m_searchObject);

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    m_circle = LoadingCircle::create();
    m_circle->setPosition({0, -55});
    m_circle->setScale(.475f);
    m_circle->show();
    m_circle->setZOrder(999);

    m_goBtn->setVisible(false);

    this->getScheduler()->scheduleSelector(schedule_selector(LevelBrowserEndLayer::onTimer), this, 1, false);
}

bool LevelBrowserEndLayer::init(LevelBrowserLayer* levelBrowserLayer){
    bool init = createBasics({230.0f, 160.0f}, menu_selector(LevelBrowserEndLayer::onClose), 0.8f);
    if(!init) return false;

    levelBrowserLayer->retain();
    m_levelBrowserLayer = levelBrowserLayer;

    auto title = createTextLabel("Finding Last Page", {0,63}, 0.7f, m_buttonMenu, "goldFont.fnt");
    title->setID("title-label"_spr);

    m_textLabel = TextArea::create("Press <cg>Go</c> to start searching.\nThis might take several\nminutes with certain\nfilter combinations.", "chatFont.fnt", 1, 295, {0,1}, 20, false);
    m_textLabel->setAnchorPoint({0,1});
    m_textLabel->setPosition({-90,44});
    m_textLabel->setID("text-label"_spr);
    m_buttonMenu->addChild(m_textLabel);

    cocos2d::extension::CCScale9Sprite* infoBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f });
    infoBg->setContentSize({320,140});
    infoBg->setColor({123,60,31});
    m_buttonMenu->addChild(infoBg, -1);
    infoBg->setPosition({0,6});
    infoBg->setScale(0.6f);
    infoBg->setID("info-background"_spr);

    auto buttonSprite = ButtonSprite::create("Go", 40, true, "goldFont.fnt", "GJ_button_01.png", 30, 0.8f);
    m_goBtn = CCMenuItemSpriteExtra::create(
        buttonSprite,
        this,
        menu_selector(LevelBrowserEndLayer::onOK)
    );
    m_goBtn->setSizeMult(1.2f);
    m_goBtn->setPosition({0,-55});
    m_goBtn->setID("go-button"_spr);
    m_buttonMenu->addChild(m_goBtn);

    m_timer = CCLabelBMFont::create("99", "bigFont.fnt");
    m_timer->setPosition({110,-73});
    m_timer->setAnchorPoint({1,0});
    m_timer->setScale(.5f);
    m_buttonMenu->addChild(m_timer);
    m_timer->setVisible(false);
    m_timer->setID("timer-label"_spr);

    return true;
}

CCLabelBMFont* LevelBrowserEndLayer::createTextLabel(const std::string text, const CCPoint& position, const float scale, CCNode* menu, const char* font){
    CCLabelBMFont* bmFont = CCLabelBMFont::create(text.c_str(), font);
    bmFont->setPosition(position);
    bmFont->setScale(scale);
    menu->addChild(bmFont);
    return bmFont;
}

void LevelBrowserEndLayer::loadLevelsFinished(cocos2d::CCArray*, const char* test){
    m_min = m_levelBrowserLayer->m_searchObject->m_page;

    if(m_max == 0) {
        m_levelBrowserLayer->m_searchObject->m_page *= 2;

        getOnlineLevels(m_levelBrowserLayer->m_searchObject);

        updateDisplay();
        return;
    }

    if(m_max - m_min == 1 && m_max != 0) {
        onClose(nullptr);
        return;
    }

    m_levelBrowserLayer->m_searchObject->m_page = m_levelBrowserLayer->m_searchObject->m_page + ((m_max - m_min) / 2);

    getOnlineLevels(m_levelBrowserLayer->m_searchObject);

    updateDisplay();
}
void LevelBrowserEndLayer::loadLevelsFinished(cocos2d::CCArray* result, const char* test, int){
    //lists overload
    loadLevelsFinished(result, test);
}
void LevelBrowserEndLayer::loadLevelsFailed(const char* test){
    m_max = m_levelBrowserLayer->m_searchObject->m_page;
    if(m_requestsToMax == 0) {
        m_requestsToMax = m_requests;
        //maxReached = std::time(nullptr);
        m_maxReached = m_lastLoad;
    }

    if(m_max - m_min == 1) {
        onClose(nullptr);
        return;
    };

    m_levelBrowserLayer->m_searchObject->m_page = m_levelBrowserLayer->m_searchObject->m_page - ((m_max - m_min) / 2);

    getOnlineLevels(m_levelBrowserLayer->m_searchObject);

    updateDisplay();
}
void LevelBrowserEndLayer::loadLevelsFailed(const char* test, int){
    //lists overload
    loadLevelsFailed(test);
}
void LevelBrowserEndLayer::setupPageInfo(gd::string, const char*){
    
}

void LevelBrowserEndLayer::updateDisplay(){
    if(!m_updateLabel) return;

    m_lastLoad = std::time(nullptr);

    auto maximumStr = m_requestsToMax <= 0 ? "" : CCString::createWithFormat(" / %i", m_requestsToMax * 2)->getCString();

    m_textLabel->setString(
        CCString::createWithFormat("<cg>Minimum</c>: %i\n<cy>Current</c>: %i\n<cr>Maximum</c>: %i\n<cl>Requests</c>: %i%s", m_min, m_levelBrowserLayer->m_searchObject->m_page, m_max, ++m_requests, maximumStr)->getCString()
    );
    m_textLabel->setScale(1.f);
}

void LevelBrowserEndLayer::onTimer(float dt) {
    if(m_requests > m_requestsToMax && m_requestsToMax > 0) {
        double timePerRequest = (double) (m_lastLoad - m_maxReached) / (m_requests - m_requestsToMax);
        int requestsRemaining = (m_requestsToMax * 2) - m_requests;
        time_t timeElapsed = std::time(nullptr) - m_lastLoad;
        m_timer->setString(fmt::format("{:.0f}", (timePerRequest * requestsRemaining) - timeElapsed).c_str());
        m_timer->setVisible(true);
    }
}

void LevelBrowserEndLayer::getOnlineLevels(GJSearchObject* searchObj) {
    auto GLM = GameLevelManager::sharedState();
    GLM->m_levelManagerDelegate = this;
    auto storedLevels = GLM->getStoredOnlineLevels(searchObj->getKey());
    if(storedLevels) {
        m_updateLabel = false;
        loadLevelsFinished(storedLevels, "");
    } else {
        m_updateLabel = true;
        this->getScheduler()->scheduleSelector(schedule_selector(LevelBrowserEndLayer::onQueueDownload), this, 1, 0, 0.25f, false);
    }
}

void LevelBrowserEndLayer::onQueueDownload(float dt) {
    auto GLM = GameLevelManager::sharedState();
    GLM->m_levelManagerDelegate = this;
    m_levelBrowserLayer->m_searchObject->m_searchMode == 1 ? GLM->getLevelLists(m_levelBrowserLayer->m_searchObject) : GLM->getOnlineLevels(m_levelBrowserLayer->m_searchObject);
}