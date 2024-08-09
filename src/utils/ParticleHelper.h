//stolen from https://github.com/Alphalaneous/Projected-Stars/blob/main/src/ParticleHelper.hpp
#pragma once
#include "Geode/cocos/cocoa/CCDictionary.h"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ParticleHelper {
public:
    
    static CCParticleSystemQuad* createStarParticles(int maxParticles) {

        auto dict = CCDictionary::createWithContentsOfFileThreadSafe("dragEffect.plist");
        if(!dict) dict = CCDictionary::create();

        dict->setObject(CCString::create("1"), "emitterType");
        dict->setObject(CCString::create("-1"), "duration");
        dict->setObject(CCString::createWithFormat("%i", maxParticles), "maxParticles");
        dict->setObject(CCString::create("1.5"), "particleLifespan");
        dict->setObject(CCString::create("0"), "angle");
        dict->setObject(CCString::create("180"), "angleVariance");
        dict->setObject(CCString::create("10"), "maxRadius");
        dict->setObject(CCString::create("30"), "minRadius");
        dict->setObject(CCString::create("0"), "sourcePositionx");
        dict->setObject(CCString::create("0"), "sourcePositiony");
        dict->setObject(CCString::create("0"), "sourcePositionVariancex");
        dict->setObject(CCString::create("0"), "sourcePositionVariancey");
        dict->setObject(CCString::create("4"), "startParticleSize");
        dict->setObject(CCString::create("4"), "finishParticleSize");
        dict->setObject(CCString::create("0"), "startParticleSizeVariance");
        dict->setObject(CCString::create("1"), "startColorRed");
        dict->setObject(CCString::create("1"), "startColorGreen");
        dict->setObject(CCString::create("1"), "startColorBlue");
        dict->setObject(CCString::create("1"), "startColorAlpha");
        dict->setObject(CCString::create("1"), "startColorVarianceRed");
        dict->setObject(CCString::create("1"), "startColorVarianceBlue");
        dict->setObject(CCString::create("1"), "startColorVarianceGreen");
        dict->setObject(CCString::create("0"), "startColorVarianceAlpha");
        dict->setObject(CCString::create("1"), "finishColorRed");
        dict->setObject(CCString::create("1"), "finishColorGreen");
        dict->setObject(CCString::create("1"), "finishColorBlue");
        dict->setObject(CCString::create("0"), "finishColorAlpha");
        dict->setObject(CCString::create("1"), "finishColorVarianceRed");
        dict->setObject(CCString::create("1"), "finishColorVarianceBlue");
        dict->setObject(CCString::create("1"), "finishColorVarianceGreen");
        dict->setObject(CCString::create("0"), "finishColorVarianceAlpha");

        auto p = CCParticleSystemQuad::create();
        p->initWithDictionary(dict, false);
        
        return p;
    }
};