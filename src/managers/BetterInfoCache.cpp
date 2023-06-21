#include "BetterInfoCache.h"
#include "../utils.hpp"

#include <fstream>

bool BetterInfoCache::init(){
    auto loadResult = load();
    if(!loadResult) {
        log::warn("Unable to load BI cache");
    }

    return true;
}

Result<> BetterInfoCache::load() {
    //copied straight from https://github.com/geode-sdk/geode/blob/9cd0327766386a6c2b347d483bb6556cae6f7f48/loader/src/loader/ModImpl.cpp#L172
    auto savedPath = Mod::get()->getSaveDir() / "cache.json";
    if (ghc::filesystem::exists(savedPath)) {
        GEODE_UNWRAP_INTO(auto data, utils::file::readString(savedPath));

        try {
            this->m_json = json::parse(data);
        } catch (std::exception& err) {
            return Err(std::string("Unable to parse cache: ") + err.what());
        }
        if (!m_json.is_object()) {
            log::warn("cache.json was somehow not an object, forcing it to one");
            m_json = json::Object();
        }
    }
}

Result<> BetterInfoCache::save() {
    //m_json["level-name-dict"] = m_levelNameDict;
    //m_json["coin-count-dict"] = m_coinCountDict;

    std::string savedStr = m_json.dump();

    auto res2 = utils::file::writeString(Mod::get()->getSaveDir() / "cache.json", savedStr);
    if (!res2) {
        log::error("Unable to save values: {}", res2.unwrapErr());
    }

    return Ok();
}

BetterInfoCache::BetterInfoCache(){}

void BetterInfoCache::checkDailies() {
    std::set<int> toDownload;

    auto GLM = GameLevelManager::sharedState();

    auto dailyLevels = GLM->m_dailyLevels;
    CCDictElement* obj;
    CCDICT_FOREACH(dailyLevels, obj){
        auto currentLvl = static_cast<GJGameLevel*>(obj->getObject());
        if(currentLvl == nullptr) continue;

        auto idString = std::to_string(currentLvl->m_levelID);
        if(m_levelNameDict.find(currentLvl->m_levelID) != m_levelNameDict.end() && m_coinCountDict.find(currentLvl->m_levelID) != m_coinCountDict.end()) continue;

        auto levelFromSaved = static_cast<GJGameLevel*>(GLM->m_onlineLevels->objectForKey(std::to_string(currentLvl->m_levelID).c_str()));
        if(levelFromSaved != nullptr) cacheLevel(levelFromSaved);
        else toDownload.insert(currentLvl->m_levelID);
    }

    if(!toDownload.empty()) cacheLevels(toDownload);

    this->save();
}

void BetterInfoCache::cacheLevel(GJGameLevel* level) {
    auto idString = std::to_string(level->m_levelID);
    m_levelNameDict[level->m_levelID] = level->m_levelName;
    m_coinCountDict[level->m_levelID] = level->m_coins;
}

void BetterInfoCache::cacheLevels(std::set<int> toDownload) {
    //Search type 10 currently does not have a limit on level IDs, so we can do this all in one request
    bool first = true;
    std::stringstream levels;
    std::vector<std::string> levelSets;
    int i = 0;
    for(const auto& id : toDownload) {
        if(i != 0) levels << ",";
        levels << id;
        first = false;
        
        i = (i + 1) % 300;
        if(i == 0) {
            levelSets.push_back(levels.str());
            levels.str("");
        }
    }

    levelSets.push_back(levels.str());

    //Splitting up the request like this is required because GJSearchObject::create crashes if str is too long
    for(const auto& set : levelSets) {
        auto searchObj = GJSearchObject::create(SearchType::MapPackOnClick, set);
        auto GLM = GameLevelManager::sharedState();
        GLM->m_onlineListDelegate = this;
        GLM->getOnlineLevels(searchObj);
    }

}

const char* BetterInfoCache::getLevelName(int levelID) {
    if(m_levelNameDict.find(levelID) == m_levelNameDict.end()) return "Unknown";
    return m_levelNameDict[levelID].c_str();
}

int BetterInfoCache::getCoinCount(int levelID) {
    if(m_coinCountDict.find(levelID) == m_coinCountDict.end()) return 3;
    return m_coinCountDict[levelID];
}

void BetterInfoCache::loadListFinished(cocos2d::CCArray* levels, const char*) {
    for(size_t i = 0; i < levels->count(); i++) {
        auto level = static_cast<GJGameLevel*>(levels->objectAtIndex(i));
        if(level == nullptr) continue;

        cacheLevel(level);
    }

    this->save();
}

void BetterInfoCache::loadListFailed(const char*) {}
void BetterInfoCache::setupPageInfo(std::string, const char*) {}
