#include <Geode/Geode.hpp>
#include <Geode/modify/GJUserScore.hpp>

#include "../utils.hpp"

using namespace geode::prelude;

class BI_DLL $modify(BIGJUserScore, GJUserScore) {
    static GJUserScore* create(cocos2d::CCDictionary* dict) {
        auto level = GJUserScore::create(dict);
        if (auto val = dict->valueForKey("62")) {
            level->setUserObject("key_62"_spr, CCInteger::create(val->intValue()));
        }
        if (auto val = dict->valueForKey("63")) {
            level->setUserObject("key_63"_spr, CCInteger::create(val->intValue()));
        }
        if (auto val = dict->valueForKey("64")) {
            level->setUserObject("key_64"_spr, CCInteger::create(val->intValue()));
        }
        return level;
    }
};
