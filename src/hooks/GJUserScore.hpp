#pragma once

#include <Geode/Bindings.hpp>
#include <Geode/modify/GJUserScore.hpp>

using namespace geode::prelude;

class $modify(BIGJUserScore, GJUserScore) {
    int m_color3 = 0;
    int m_moons = 0;
    int m_accSwing = 0;
    int m_accJetpack = 0;

    static GJUserScore* create(cocos2d::CCDictionary* dict) {
        auto score = static_cast<BIGJUserScore*>(GJUserScore::create(dict));

        score->m_fields->m_color3 = dict->valueForKey("51")->intValue();
        score->m_fields->m_moons = dict->valueForKey("52")->intValue();
        score->m_fields->m_accSwing = dict->valueForKey("53")->intValue();
        score->m_fields->m_accJetpack = dict->valueForKey("54")->intValue();

        return score;
    }
};