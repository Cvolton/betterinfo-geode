#include "QuestHistoryPopup.h"
#include "QuestHistoryObject.h"
#include "QuestHistoryCell.h"
#include "../_bases/CvoltonListView.h"

QuestHistoryPopup* QuestHistoryPopup::create(){
    auto ret = new QuestHistoryPopup();
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

bool QuestHistoryPopup::init(){
    if(!CvoltonAlertLayerStub::init({380.0f, 210.0f}, .9f, 0x4B, 2)) return false;
    m_closeBtn->setVisible(false);

    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto GSM = GameStatsManager::sharedState();

    std::map<int, int> completed;
    size_t completedSum = 0;

    for(auto [key, value] : CCDictionaryExt<gd::string, cocos2d::CCString*>(GSM->m_challengeDiamonds)) {
        if(key.size() < 1 || key[0] != 'c') continue;

        completed[value->intValue()] += 1;
        completedSum += 1;
    }

    auto counts = CCArray::create();
    for(auto [key, value] : completed) {
        //log::info("Completed challenge {} diamonds {} times", key, value);
        counts->addObject(QuestHistoryObject::create(key, value));
    }

    auto listView = CvoltonListView<QuestHistoryCell>::create(counts, 272, 128, 44.f);
    
    auto listLayer = GJCommentListLayer::create(listView, "Quest History", {255, 0, 0}, 272, 128, true);
    listLayer->setPosition((winSize / 2) - (listLayer->getContentSize() / 2) - CCPoint(6, 0));
    m_mainLayer->addChild(listLayer);

    listLayer->getChildByID("top-border")->setScaleX(0.8f);
    listLayer->getChildByID("bottom-border")->setScaleX(0.8f);

    listLayer->getChildByID("right-border")->setScaleX(0.8f);
    listLayer->getChildByID("right-border")->setPositionX(277.65);

    listLayer->getChildByID("left-border")->setScaleX(0.8f);
    listLayer->getChildByID("left-border")->setPositionX(-5.45);

    auto scrollbar = Scrollbar::create(listView->m_tableView);
    scrollbar->setPosition({(winSize.width / 2) + (listLayer->getScaledContentSize().width / 2) + 10 - 6, winSize.height / 2});
    m_mainLayer->addChild(scrollbar);

    auto buttonSprite = ButtonSprite::create("OK", 0, false, "goldFont.fnt", "GJ_button_01.png", 0, .9f);
    buttonSprite->setScale(.85f);
    auto buttonButton = CCMenuItemSpriteExtra::create(
        buttonSprite,
        this,
        menu_selector(QuestHistoryPopup::onClose)
    );
    buttonButton->setSizeMult(1.2f);
    buttonButton->setPosition({0, -83});
    buttonButton->setID("ok-button"_spr);
    m_buttonMenu->addChild(buttonButton);

    auto totalText = CCLabelBMFont::create(
        fmt::format("Total: {} Quests", completedSum).c_str(),
        "goldFont.fnt"
    );
    totalText->setPosition({winSize.width / 2, winSize.height / 2 + 84});
    totalText->setScale(.7f);
    m_mainLayer->addChild(totalText);

    return true;
}

void QuestHistoryPopup::show() {
    reinterpret_cast<InfoLayer*>(this)->InfoLayer::show();
}