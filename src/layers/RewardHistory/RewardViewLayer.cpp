#include "RewardViewLayer.h"
#include "../JumpToPageLayer.h"
#include "../../utils.hpp"

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
    return item1->m_chestID < item2->m_chestID;
}

bool RewardViewLayer::init(CCDictionary* chests, const char* title) {
    Mod::get()->setSavedValue<std::string>("reward-cell-title", title);
    m_title = fmt::format("{} Chests", title);

    m_data = CCArray::create();
    m_data->retain();
    CCDictElement* obj;
    CCDICT_FOREACH(chests, obj){
        auto currentReward = static_cast<GJRewardItem*>(obj->getObject());
        if(currentReward != nullptr) m_data->addObject(currentReward);
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