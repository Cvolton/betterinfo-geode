#include "UnregisteredProfileLayer.h"
#include "../utils.hpp"

UnregisteredProfileLayer* UnregisteredProfileLayer::create(GJUserScore* score, CCNode* invoker){
    auto ret = new UnregisteredProfileLayer();
    if (ret && ret->init(score, invoker)) {
        //robert 1 :D
        ret->autorelease();
    } else {
        //robert -1
        delete ret;
        ret = nullptr;
    }
    return ret;
}

void UnregisteredProfileLayer::onClose(CCObject* sender)
{
    m_score->release();
    if(m_invoker != nullptr) m_invoker->release();
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void UnregisteredProfileLayer::onCopyUserID(CCObject* sender)
{
    BetterInfo::copyToClipboard(std::to_string(m_score->m_userID).c_str(), this);
}

void UnregisteredProfileLayer::onCopyPlayerName(CCObject* sender)
{
    BetterInfo::copyToClipboard(m_score->m_userName.c_str(), this);
}

void UnregisteredProfileLayer::displayProfile(int userID, gd::string userName, CCNode* invoker){
    GJUserScore* score = GJUserScore::create();
    score->m_userID = userID;
    score->m_userName = userName;
    score->m_iconType = IconType::Cube;
    score->m_iconID = 0;
    score->m_color1 = 0;
    score->m_color2 = 3;
    UnregisteredProfileLayer* profileLayer = UnregisteredProfileLayer::create(score, invoker);
    profileLayer->show();
}

void UnregisteredProfileLayer::doSearch(int userID) {
    auto searchObject = GJSearchObject::create(SearchType::UsersLevels, std::to_string(userID));//, contentStream.str());
    auto browserLayer = LevelBrowserLayer::scene(searchObject);

    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);
    
    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void UnregisteredProfileLayer::onMyLevels(CCObject* sender) {
    doSearch(m_score->m_userID);
}

void UnregisteredProfileLayer::onCommentHistory(CCObject* sender) {
    InfoLayer* infoLayer = InfoLayer::create(nullptr, m_score, nullptr);
    infoLayer->m_scene = m_invoker;
    infoLayer->show();
    onClose(sender);
}

bool UnregisteredProfileLayer::init(GJUserScore* score, CCNode* invoker){
    bool init = createBasics({360,180}, menu_selector(UnregisteredProfileLayer::onClose));
    if(!init) return false;

    score->retain();
    m_score = score;
    if(invoker != nullptr) invoker->retain();
    m_invoker = invoker;

    createTitle("");

    auto userNameText = CCLabelBMFont::create(m_score->m_userName.c_str(), "bigFont.fnt");
    auto userNameBtn = CCMenuItemSpriteExtra::create(
        userNameText,
        this,
        menu_selector(UnregisteredProfileLayer::onCopyPlayerName)
    );
    userNameBtn->setPosition({0, (m_alertSize.y/2) - 22});
    userNameBtn->setID("username-button"_spr);
    m_buttonMenu->addChild(userNameBtn);

    auto GM = GameManager::sharedState();

    SimplePlayer* icon = SimplePlayer::create(m_score->m_iconID);
    icon->updatePlayerFrame(m_score->m_iconID, m_score->m_iconType);
    icon->setColor(GM->colorForIdx(m_score->m_color1));
    icon->setSecondColor(GM->colorForIdx(m_score->m_color2));
    icon->updateColors();

    //icon->setPosition({440,224});
    icon->setPosition({155, (m_alertSize.y/2) - 24});
    icon->setID("simpleplayer"_spr);
    m_buttonMenu->addChild(icon);

    auto myLevelsBtn = createButton(
        "accountBtn_myLevels_001.png",
        {154, -62},
        menu_selector(UnregisteredProfileLayer::onMyLevels),
        0.8f
    );
    myLevelsBtn->setID("my-levels-button"_spr);

    auto levelsText = CCSprite::createWithSpriteFrameName("GJ_myLevelsTxt_001.png");
    levelsText->setScale(0.8f);
    levelsText->setPosition({104, -62});
    levelsText->setID("my-levels-text"_spr);
    m_buttonMenu->addChild(levelsText);

    auto commentBtn = createButton(
        "GJ_chatBtn_001.png",
        {154, 0},
        menu_selector(UnregisteredProfileLayer::onCommentHistory),
        0.8f
    );
    commentBtn->setID("comment-button"_spr);

    std::ostringstream userIDText;
    userIDText << "User ID: " << m_score->m_userID << "\nAccount ID: None";

    auto userIDTextNode = CCLabelBMFont::create(userIDText.str().c_str(), "bigFont.fnt");
    userIDTextNode->setAnchorPoint({0,0});
    userIDTextNode->setScale(0.45f);
    userIDTextNode->setID("userid-text-node"_spr);
    auto userIDBtn = CCMenuItemSpriteExtra::create(
        userIDTextNode,
        this,
        menu_selector(UnregisteredProfileLayer::onCopyUserID)
    );
    userIDBtn->setPosition({-104, -66.5f});
    userIDBtn->setID("userid-button"_spr);
    //userIDBtn->setPosition({-168, -75.5f});
    m_buttonMenu->addChild(userIDBtn);

    return true;
}