#include <Geode/Bindings.hpp>
#include <Geode/modify/DailyLevelNode.hpp>

using namespace geode::prelude;

class $modify(BIDailyLevelNode, DailyLevelNode) {
    bool init(GJGameLevel* level, DailyLevelPage* page, bool weekly) {
        if(!DailyLevelNode::init(level, page, weekly)) return false;

        for(auto node : CCArrayExt<CCNode>(this->getChildren())) {
            if(node->getPosition() == CCPoint(182, 53)) node->setPositionX(130);
        }

        return true;
    }
};