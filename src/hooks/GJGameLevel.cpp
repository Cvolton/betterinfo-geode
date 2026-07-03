#include <Geode/Geode.hpp>
#include <Geode/modify/GJGameLevel.hpp>

#include "../utils.hpp"

using namespace geode::prelude;

class BI_DLL $modify(BIGJGameLevel, GJGameLevel) {
    static GJGameLevel* create(cocos2d::CCDictionary* dict, bool download) {
        auto level = GJGameLevel::create(dict, download);
        if (auto val = dict->valueForKey("62")) {
            level->setUserObject("key_62"_spr, CCInteger::create(val->intValue()));
        }
        if (auto val = dict->valueForKey("63")) {
            level->setUserObject("key_63"_spr, CCInteger::create(val->intValue()));
        }
        return level;
    }
};
