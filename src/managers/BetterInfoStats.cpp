#include "BetterInfoStats.h"
#include "../utils.hpp"

#include <fmt/format.h>
#include <cvolton.level-id-api/include/EditorIDs.hpp>

bool BetterInfoStats::init(){
    bool init = CCNode::init();
    if(!init) return false;

    m_fileName = fmt::format("geode/mods/{}/CCBetterInfoStats.dat", Mod::get()->getID());
    
    this->migrateSaveData();
    this->setup();

    return true;
}

void BetterInfoStats::save() {
    m_saved = false;
    GManager::save();
}

void BetterInfoStats::migrateSaveData() {
    auto oldPath = dirs::getSaveDir() / "CCBetterInfoStats.dat";
    auto newPath = dirs::getSaveDir() / std::string(this->m_fileName); //it would be better to utilize Mod::get()->getSaveDir, however that can result in future issues

    if(std::filesystem::exists(oldPath) && !std::filesystem::exists(newPath)) {
        log::info("CCBetterInfoStats exists in main GD folder but not in mod folder, migrating");
        std::filesystem::rename(oldPath, newPath);
        
        this->getScheduler()->scheduleSelector(schedule_selector(BetterInfoStats::migrationPopup), this, 1, 0, 3, false);
    }
}

void BetterInfoStats::migrationPopup(float dt){
    FLAlertLayer::create("BetterInfo", "<cg>Succesfully</c> loaded stats from a previous installation of <c_>Better</c><cj>Info</c>.\n\nPlease note that if you choose to remove your data while uninstalling this version of BI, you will lose these stats permanently.", "OK")->show();
}

BetterInfoStats::BetterInfoStats(){}

void BetterInfoStats::encodeDataTo(DS_Dictionary* data) {
    data->setDictForKey("normalCompletions", m_normalDict);
    data->setDictForKey("practiceCompletions", m_practiceDict);
    data->setDictForKey("firstPlayed", m_firstPlayedDict);
    data->setDictForKey("lastPlayed", m_lastPlayedDict);
    data->setDictForKey("normalAttempt", m_normalAttemptDict);
    data->setDictForKey("practiceAttempt", m_practiceAttemptDict);
    data->setDictForKey("listFolderAssignments", m_listFolderAssignmentsDict);
}

void BetterInfoStats::dataLoaded(DS_Dictionary* data) {
    auto normalDict = static_cast<CCDictionary*>(data->getObjectForKey("normalCompletions"));
    if(normalDict) m_normalDict = normalDict;

    auto practiceDict = static_cast<CCDictionary*>(data->getObjectForKey("practiceCompletions"));
    if(practiceDict) m_practiceDict = practiceDict;

    auto firstPlayedDict = static_cast<CCDictionary*>(data->getObjectForKey("firstPlayed"));
    if(firstPlayedDict) m_firstPlayedDict = firstPlayedDict;

    auto lastPlayedDict = static_cast<CCDictionary*>(data->getObjectForKey("lastPlayed"));
    if(lastPlayedDict) m_lastPlayedDict = lastPlayedDict;

    auto normalAttemptDict = static_cast<CCDictionary*>(data->getObjectForKey("normalAttempt"));
    if(normalAttemptDict) m_normalAttemptDict = normalAttemptDict;

    auto practiceAttemptDict = static_cast<CCDictionary*>(data->getObjectForKey("practiceAttempt"));
    if(practiceAttemptDict) m_practiceAttemptDict = practiceAttemptDict;

    auto listFolderAssignmentsDict = static_cast<CCDictionary*>(data->getObjectForKey("listFolderAssignments"));
    if(listFolderAssignmentsDict) m_listFolderAssignmentsDict = listFolderAssignmentsDict;

    this->establishListFolders();
    this->save();
}

void BetterInfoStats::firstLoad() {
    this->save();
}

void BetterInfoStats::establishListFolders() {
    for(auto list : CCArrayExt<GJLevelList*>(LocalLevelManager::sharedState()->m_localLists)) {
        list->m_folder = this->getListFolder(list);
    }
    
    for(auto [key, list] : CCDictionaryExt<gd::string, GJLevelList*>(GameLevelManager::sharedState()->m_favoriteLists)) {
        list->m_folder = this->getListFolder(list);
    }
}

void BetterInfoStats::logCompletion(GJGameLevel* level, bool practice) {
    if(getCompletion(level, practice) != 0) return;

    logCompletion(level, practice, std::time(nullptr));
}

void BetterInfoStats::logCompletion(GJGameLevel* level, bool practice, time_t timestamp) {
    auto dict = practice ? m_practiceDict : m_normalDict;
    dict->setObject(CCString::create(std::to_string(timestamp).c_str()), keyForLevel(level));
    
    this->save();
}

time_t BetterInfoStats::getCompletion(GJGameLevel* level, bool practice) {
    auto dict = practice ? m_practiceDict : m_normalDict;

    auto ret = dict->valueForKey(keyForLevel(level));
    if(std::string_view(ret->getCString()).empty()) return 0;

    return BetterInfo::strtol(ret->getCString());
}

void BetterInfoStats::logPlay(GJGameLevel* level) {
    auto idString = keyForLevel(level);
    auto timeString = CCString::create(std::to_string(std::time(nullptr)).c_str());
    m_lastPlayedDict->setObject(timeString, idString);
    if(getPlay(level, false) == 0 && level->m_normalPercent <= 0) m_firstPlayedDict->setObject(timeString, idString);
    if(level->m_normalPercent == 100 && getCompletion(level, false) == 0) logCompletion(level, false, -1);
    if(level->m_practicePercent == 100 && getCompletion(level, true) == 0) logCompletion(level, true, -1);
    
    this->save();
}

time_t BetterInfoStats::getPlay(GJGameLevel* level, bool last) {
    auto dict = last ? m_lastPlayedDict : m_firstPlayedDict;

    auto ret = dict->valueForKey(keyForLevel(level));
    if(std::string_view(ret->getCString()).empty()) return 0;

    return BetterInfo::strtol(ret->getCString());
}

std::string BetterInfoStats::keyForLevel(GJGameLevel* level) {
    auto type = static_cast<int>(level->m_levelType);
    if(!type) type = 3;

    return fmt::format("{}_{}_{}_{}", EditorIDs::getID(level), type, level->m_dailyID.value(), level->m_gauntletLevel);
}

void BetterInfoStats::logAttempt(GJGameLevel* level, bool practice) {
    auto dict = practice ? m_practiceAttemptDict : m_normalAttemptDict;
    dict->setObject(CCString::create(std::to_string(getAttempts(level, practice) + 1).c_str()), keyForLevel(level));
}

int BetterInfoStats::getAttempts(GJGameLevel* level, bool practice) {
    auto dict = practice ? m_practiceAttemptDict : m_normalAttemptDict;

    auto ret = dict->valueForKey(keyForLevel(level));
    if(std::string_view(ret->getCString()).empty()) return 0;

    return BetterInfo::stoi(ret->getCString());
}

std::string BetterInfoStats::keyForList(GJLevelList* list) {
    return fmt::format("{}_{}", (int) list->m_listType, EditorIDs::getID(list));
}

void BetterInfoStats::setListFolder(GJLevelList* list, int folder) {
    m_listFolderAssignmentsDict->setObject(CCString::create(std::to_string(folder)), keyForList(list));
    
    this->save();
}

int BetterInfoStats::getListFolder(GJLevelList* list) {
    auto ret = m_listFolderAssignmentsDict->valueForKey(keyForList(list));
    if(std::string_view(ret->getCString()).empty()) return 0;

    return BetterInfo::stoi(ret->getCString());
}