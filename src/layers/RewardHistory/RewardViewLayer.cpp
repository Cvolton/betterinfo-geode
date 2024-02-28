#include "RewardViewLayer.h"
#include "RewardCell.h"
#include "../../hooks/GJRewardItem.h"

using namespace gd;
using namespace cocos2d;

RewardViewLayer* RewardViewLayer::create(CCDictionary* chests, const char* title) {
    auto ret = new RewardViewLayer();
    if (ret && ret->init(chests, title)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool RewardViewLayer::compareRewards(const void* i1, const void* i2){
    const GJRewardItem* item1 = reinterpret_cast<const GJRewardItem*>(i1);
    const GJRewardItem* item2 = reinterpret_cast<const GJRewardItem*>(i2);
    if(item1->m_chestID > 0 || item2->m_chestID > 0) return item1->m_chestID < item2->m_chestID;

    auto key1 = BIGJRewardItem::s_keys.contains(item1) ? BIGJRewardItem::s_keys.at(item1) : std::string("");
    auto key2 = BIGJRewardItem::s_keys.contains(item2) ? BIGJRewardItem::s_keys.at(item2) : std::string("");
    return key1 < key2;
}

bool RewardViewLayer::init(CCDictionary* chests, const char* title) {
    Mod::get()->setSavedValue<std::string>("reward-cell-title", title);
    m_title = fmt::format("{} Chests", title);
    setData(CCArray::create());

    m_noInternetText = fmt::format("You have not <cg>opened</c> any\n<cl>{} Chests</c> yet!", title);
    m_showNoInternet = true;

    for(auto [key, reward] : CCDictionaryExt<gd::string, GJRewardItem*>(chests)) {
        if(reward == nullptr) continue;

        static_cast<BIGJRewardItem*>(reward)->setKey(key);
        m_data->addObject(reward);
    }
    std::sort(m_data->data->arr, m_data->data->arr + m_data->data->num, RewardViewLayer::compareRewards);

    BIViewLayer::init();

    return true;
}

void RewardViewLayer::loadPage(){
    auto displayedLevels = trimData();

    m_listView = CvoltonListView<RewardCell>::create(displayedLevels, 356.f, 220.f, 45.f);

    BIViewLayer::loadPage();
}

CCScene* RewardViewLayer::scene(CCDictionary* chests, const char* title) {
    auto layer = RewardViewLayer::create(chests, title);
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}