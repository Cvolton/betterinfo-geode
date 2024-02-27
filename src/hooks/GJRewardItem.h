#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/GJRewardItem.hpp>

#include "../utils.hpp"

using namespace geode::prelude;

class $modify(BIGJRewardItem, GJRewardItem) {
    static inline std::unordered_map<const GJRewardItem*, std::string> s_keys;

    void setKey(const std::string& key) {
        s_keys[this] = key;
    }

    std::string getKey() {
        return s_keys[this];
    }

    int getKeyInt() {
        return BetterInfo::stoi(getKey());
    }
};