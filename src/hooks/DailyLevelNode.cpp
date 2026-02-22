#include <Geode/Geode.hpp>
#include <Geode/modify/DailyLevelNode.hpp>

#include "../utils.hpp"

using namespace geode::prelude;

class BI_DLL $modify(BIDailyLevelNode, DailyLevelNode) {
    bool init(GJGameLevel* level, DailyLevelPage* page, bool weekly) {
        if(!DailyLevelNode::init(level, page, weekly)) return false;

        if(auto newNode = getChildByID("new-label")) {
            if(newNode->getPositionX() == 182) newNode->setPositionX(130);
        }

        if(auto robtopTimelyNode = getChildByID("timely-id-label")) {
            robtopTimelyNode->setVisible(false);
        }

        return true;
    }
};