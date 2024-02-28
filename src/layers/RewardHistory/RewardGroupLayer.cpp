#include "RewardGroupLayer.h"
#include "RewardCategoryCell.h"

using namespace gd;
using namespace cocos2d;

RewardGroupLayer* RewardGroupLayer::create(const char* title, CCDictionary* chests, RewardGroupCategoryVector categories) {
    auto ret = new RewardGroupLayer();
    if (ret && ret->init(title, chests, categories)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool RewardGroupLayer::init(const char* title, CCDictionary* chests, RewardGroupCategoryVector categories) {
    m_title = fmt::format("{} Chests", title);
    setData(CCArray::create());

    m_noInternetText = fmt::format("You have not <cg>opened</c> any <cl>{} Chests</c> yet", title);
    m_showNoInternet = true;

    for (auto& [title, icon, filter] : categories) {
        auto category = RewardCategoryObject::create(title, icon, chests, filter);
        if(category->filteredChests()->count() > 0) m_data->addObject(category);
    }

    BIViewLayer::init();

    return true;
}

void RewardGroupLayer::loadPage(){
    auto displayedLevels = trimData();

    m_listView = CvoltonListView<RewardCategoryCell>::create(displayedLevels, 356.f, 220.f, 45.f);

    BIViewLayer::loadPage();
}

CCScene* RewardGroupLayer::scene(const char* title, CCDictionary* chests, RewardGroupCategoryVector categories) {
    auto layer = RewardGroupLayer::create(title, chests, categories);
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}