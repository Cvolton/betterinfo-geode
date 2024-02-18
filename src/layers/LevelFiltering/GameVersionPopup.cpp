#include "GameVersionPopup.h"

#include "../../utils.hpp"
#include "../../ui/EnumSelectNode.h"

GameVersionPopup* GameVersionPopup::create(IDRangeDelegate* delegate, int min, int max, int additional){
    auto ret = new GameVersionPopup();
    if (ret && ret->init(delegate, min, max, additional)) {
        //robert 1 :D
        ret->autorelease();
    } else {
        //robert -1
        delete ret;
        ret = nullptr;
    }
    return ret;
}

void GameVersionPopup::onClose(cocos2d::CCObject* sender)
{
    m_delegate->onIDRangeFinished(m_min, m_max, m_additional);

    CvoltonAlertLayerStub::onClose(sender);
}

bool GameVersionPopup::init(IDRangeDelegate* delegate, int min, int max, int additional){
    if(!CvoltonAlertLayerStub::init({240.0f, 150.0f}, .8f, 0x96)) return false;

    m_min = min;
    m_max = max;
    m_additional = additional;
    m_delegate = delegate;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto title = createTitle("Game Version", .45f, .9f);
    title->setScale(.7f);
    title->setPosition({0, 50});

    auto minTitle = CCLabelBMFont::create("Minimum:", "goldFont.fnt");
    minTitle->setScale(.5f);
    minTitle->setPosition({winSize.width / 2, (winSize.height / 2) + 18});
    m_mainLayer->addChild(minTitle);

    auto minNode = EnumSelectNode<int>::create({
        {0, "Undefined (0)"},
        {1, "Pre-1.7"},
        {10, "1.7"},
        {11, "Early 1.8"},
        {18, "1.8"},
        {19, "1.9"},
        {20, "2.0"},
        {21, "2.1"},
        {22, "2.2"},
        {23, "Post-2.2"}
    }, [this](int selected){
        m_min = selected;
    });
    minNode->setContentSize({200, 30});
    minNode->setPosition({winSize.width / 2, (winSize.height / 2) - 2});
    minNode->setCurrentValue(m_min);
    m_mainLayer->addChild(minNode);

    auto maxTitle = CCLabelBMFont::create("Maximum:", "goldFont.fnt");
    maxTitle->setScale(.5f);
    maxTitle->setPosition({winSize.width / 2, (winSize.height / 2) - 30});
    m_mainLayer->addChild(maxTitle);

    auto maxNode = EnumSelectNode<int>::create({
        {0, "Undefined (0)"},
        {7, "Pre-1.7"},
        {10, "1.7"},
        {11, "Early 1.8"},
        {18, "1.8"},
        {19, "1.9"},
        {20, "2.0"},
        {21, "2.1"},
        {22, "2.2"},
        {INT_MAX, "Post-2.2"}
    }, [this](int selected){
        m_max = selected;
    });
    maxNode->setContentSize({200, 30});
    maxNode->setPosition({winSize.width / 2, (winSize.height / 2) - 50});
    maxNode->setCurrentValue(m_max);
    m_mainLayer->addChild(maxNode);

    return true;
}