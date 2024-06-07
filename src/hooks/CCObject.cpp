#include <Geode/Geode.hpp>
#include <Geode/modify/CCObject.hpp>

#include "../utils.hpp"
#include "../layers/QuestHistory/QuestHistoryPopup.h"

using namespace geode::prelude;

class BI_DLL $modify(BICCObject, CCObject) {
    void release() {
        if(thread::getName() != "Main") log::info("release {} - {}", thread::getName(), typeid(*this).name() + 6);

        CCObject::release();
    }

    void retain() {
        if(thread::getName() != "Main") log::info("retain {} - {}", thread::getName(), typeid(*this).name() + 6);

        CCObject::retain();
    }
};