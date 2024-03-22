#include "LevelBrowserEndLayer.h"
#include "../utils/TimeUtils.h"

LevelBrowserEndLayer* LevelBrowserEndLayer::create(LevelBrowserLayer* levelBrowserLayer, InfoLayer* infoLayer){
    auto ret = new LevelBrowserEndLayer();
    if (ret && ret->init(levelBrowserLayer, infoLayer)) {
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
    if(m_levelBrowserLayer) {
        m_levelBrowserLayer->m_searchObject->m_page = m_min;
        m_levelBrowserLayer->loadPage(m_levelBrowserLayer->m_searchObject);
    }

    if(m_infoLayer) {
        m_infoLayer->m_page = m_min;
        m_infoLayer->loadPage(m_infoLayer->m_page, false);
    }

    if(m_circle) m_circle->fadeAndRemove();

    auto GLM = GameLevelManager::sharedState();

    CvoltonAlertLayerStub::onClose(sender);
}

void LevelBrowserEndLayer::onOK(cocos2d::CCObject* sender){
    if(m_levelBrowserLayer) m_levelBrowserLayer->m_searchObject->m_page = 1;
    if(m_infoLayer) m_infoLayer->m_page = 1;
    
    getOnlineLevels();

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    m_circle = LoadingCircle::create();
    m_circle->setPosition({0, -55});
    m_circle->setScale(.475f);
    m_circle->show();
    m_circle->setZOrder(999);

    m_goBtn->setVisible(false);

    this->getScheduler()->scheduleSelector(schedule_selector(LevelBrowserEndLayer::onTimer), this, 0.5f, false);
}

bool LevelBrowserEndLayer::init(LevelBrowserLayer* levelBrowserLayer, InfoLayer* infoLayer){
    if(!CvoltonAlertLayerStub::init({230.0f, 160.0f}, 0.8f)) return false;

    m_levelBrowserLayer = levelBrowserLayer;
    m_infoLayer = infoLayer;
    
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

    GameLevelManager::sharedState()->m_levelManagerDelegate = this;
    GameLevelManager::sharedState()->m_levelCommentDelegate = this;

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
    if(m_levelBrowserLayer) m_min = m_levelBrowserLayer->m_searchObject->m_page;
    if(m_infoLayer) m_min = m_infoLayer->m_page;

    if(m_max == 0) {
        if(m_levelBrowserLayer) m_levelBrowserLayer->m_searchObject->m_page *= 2;
        if(m_infoLayer) m_infoLayer->m_page *= 2;

        getOnlineLevels();

        updateDisplay();
        return;
    }

    if(m_max - m_min == 1 && m_max != 0) {
        onClose(nullptr);
        return;
    }

    if(m_levelBrowserLayer) m_levelBrowserLayer->m_searchObject->m_page = m_levelBrowserLayer->m_searchObject->m_page + ((m_max - m_min) / 2);
    if(m_infoLayer) m_infoLayer->m_page = m_infoLayer->m_page + ((m_max - m_min) / 2);

    getOnlineLevels();

    updateDisplay();
}
void LevelBrowserEndLayer::loadLevelsFinished(cocos2d::CCArray* result, const char* test, int){
    //lists overload
    loadLevelsFinished(result, test);
}
void LevelBrowserEndLayer::loadLevelsFailed(const char* test){

    if(m_levelBrowserLayer) m_max = m_levelBrowserLayer->m_searchObject->m_page;
    if(m_infoLayer) m_max = m_infoLayer->m_page;

    if(m_requestsToMax == 0) {
        m_requestsToMax = m_requests;
        //maxReached = std::time(nullptr);
        m_maxReached = m_lastLoad;
    }

    if(m_max - m_min == 1) {
        onClose(nullptr);
        return;
    };

    if(m_levelBrowserLayer) m_levelBrowserLayer->m_searchObject->m_page = m_levelBrowserLayer->m_searchObject->m_page - ((m_max - m_min) / 2);
    if(m_infoLayer) m_infoLayer->m_page = m_infoLayer->m_page - ((m_max - m_min) / 2);

    getOnlineLevels();

    updateDisplay();
}
void LevelBrowserEndLayer::loadLevelsFailed(const char* test, int){
    //lists overload
    loadLevelsFailed(test);
}
void LevelBrowserEndLayer::setupPageInfo(gd::string, const char*){
    
}

void LevelBrowserEndLayer::loadCommentsFinished(cocos2d::CCArray* result, char const* test) {
    loadLevelsFinished(result, test);
}

void LevelBrowserEndLayer::loadCommentsFailed(char const* test) {
    loadLevelsFailed(test);
}

void LevelBrowserEndLayer::updateDisplay(){
    if(!m_updateLabel) return;

    m_lastLoad = std::time(nullptr);

    auto maximumStr = m_requestsToMax <= 0 ? "" : CCString::createWithFormat(" / %i", m_requestsToMax * 2)->getCString();

    int page = 0;
    if(m_levelBrowserLayer) page = m_levelBrowserLayer->m_searchObject->m_page;
    if(m_infoLayer) page = m_infoLayer->m_page;

    m_textLabel->setString(
        CCString::createWithFormat("<cg>Minimum</c>: %i\n<cy>Current</c>: %i\n<cr>Maximum</c>: %i\n<cl>Requests</c>: %i%s", m_min, page, m_max, ++m_requests, maximumStr)->getCString()
    );
    m_textLabel->setScale(1.f);
    onTimer(0);
}

void LevelBrowserEndLayer::onTimer(float dt) {
    if(m_requests > m_requestsToMax && m_requestsToMax > 0) {
        double timePerRequest = (double) (m_lastLoad - m_maxReached) / (m_requests - m_requestsToMax);
        int requestsRemaining = (m_requestsToMax * 2) - m_requests;
        time_t timeElapsed = std::time(nullptr) - m_lastLoad;
        double displayTime = ((timePerRequest * requestsRemaining) - timeElapsed) + 1;
        m_timer->setString(TimeUtils::minutesSeconds(displayTime).c_str());
        m_timer->setVisible(true);
    }
}

void LevelBrowserEndLayer::getOnlineLevels() {
    auto GLM = GameLevelManager::sharedState();

    CCArray* storedLevels = nullptr;

    if(m_levelBrowserLayer) {
        storedLevels = GLM->getStoredOnlineLevels(m_levelBrowserLayer->m_searchObject->getKey());
    } else if(m_infoLayer) {
        storedLevels = GLM->getStoredOnlineLevels(GLM->getCommentKey(m_infoLayer->getID(), m_infoLayer->m_page, GameManager::sharedState()->getGameVariable("0069"), m_infoLayer->m_mode).c_str());
    }

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
    if(m_levelBrowserLayer) {
        m_levelBrowserLayer->m_searchObject->m_searchMode == 1 ? GLM->getLevelLists(m_levelBrowserLayer->m_searchObject) : GLM->getOnlineLevels(m_levelBrowserLayer->m_searchObject);
    } else if(m_infoLayer) {
        GLM->getLevelComments(m_infoLayer->getID(), m_infoLayer->m_page, m_infoLayer->m_pageEndIdx, GameManager::sharedState()->getGameVariable("0069"), m_infoLayer->m_mode);
    }
}

LevelBrowserEndLayer::~LevelBrowserEndLayer(){
    auto GLM = GameLevelManager::sharedState();

    if(GLM->m_levelManagerDelegate == this) GLM->m_levelManagerDelegate = nullptr;
    if(GLM->m_levelCommentDelegate == this) GLM->m_levelCommentDelegate = nullptr;
}