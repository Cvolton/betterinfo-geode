#include <Geode/Geode.hpp>
#include <Geode/modify/DailyLevelNode.hpp>

using namespace geode::prelude;

class $modify(BIDailyLevelNode, DailyLevelNode) {
    bool init(GJGameLevel* level, DailyLevelPage* page, bool weekly) {
        if(!DailyLevelNode::init(level, page, weekly)) return false;

        if(auto newNode = getChildByID("new-label")) {
            if(newNode->getPositionX() == 182) newNode->setPositionX(130);
        }

        return true;
    }
};