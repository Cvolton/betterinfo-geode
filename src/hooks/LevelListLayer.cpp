#include <Geode/Geode.hpp>
#include <Geode/modify/LevelListLayer.hpp>

#include "../utils.hpp"

#include "../managers/BetterInfoCache.h"

using namespace geode::prelude;

class BI_DLL 
$modify(BILevelListLayer, LevelListLayer) {

    void onClaimReward(CCObject* sender){
        LevelListLayer::onClaimReward(sender);

        BetterInfoCache::sharedState()->removeClaimedLists();
    }
};