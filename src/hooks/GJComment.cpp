#include <Geode/Geode.hpp>
#include <Geode/modify/GJComment.hpp>

#include "../utils.hpp"

using namespace geode::prelude;

class BI_DLL $modify(BIGJComment, GJComment) {
    static GJComment* create(cocos2d::CCDictionary* dict) {
        auto level = GJComment::create(dict);
        if (auto val = dict->valueForKey("15")) {
            level->setUserObject("key_15"_spr, CCInteger::create(val->intValue()));
        }
        return level;
    }
};
