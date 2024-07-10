#include <Geode/Geode.hpp>
#include <Geode/modify/GameLevelManager.hpp>

#include "../enums.hpp"
#include "../utils.hpp"
#include "../managers/BetterInfoCache.h"

using namespace geode::prelude;

class BI_DLL $modify(GameLevelManager) {
    struct Fields {
        bool skipSavedFilter = false;
        Ref<CCArray> filteredLevels = nullptr;
    };

    /*
     * Helpers
     */

    bool validateRangeOption(const std::string& option, int value, int defaultMin = 0, int defaultMax = 0) {
        if(Mod::get()->getSavedValue<bool>(option)) {
            int min = Mod::get()->getSavedValue<int>(fmt::format("{}_min", option), defaultMin);
            int max = Mod::get()->getSavedValue<int>(fmt::format("{}_max", option), defaultMax);
            if(min != 0 && value < min) return false;
            if(max != 0 && value > max) return false;
        }
        return true;
    }

    /*
     * Hooks
     */

    gd::string userNameForUserID(int userID) {
        auto userName = GameLevelManager::userNameForUserID(userID);
        auto userNameStd = std::string(userName);

        if(Mod::get()->getSettingValue<bool>("fix-green-users") && (userNameStd == "" || userNameStd == "-" || userNameStd == "Player")){

            auto newUserName = BetterInfoCache::sharedState()->getUserName(userID);
            if(userID == 32471) userName = "PixelCube"; //previous dataset had an error
            if(!newUserName.empty()) userName = newUserName;
        }

        return userName;
    }

    CCArray* getCompletedLevels(bool newFilter){
        auto BICache = BetterInfoCache::sharedState();
        CompleteMode mode = static_cast<CompleteMode>(Mod::get()->getSavedValue<int>("search_completed"));
        if(mode == CompleteMode::modeDefault) return GameLevelManager::getCompletedLevels(newFilter);

        int percentageMin = Mod::get()->getSavedValue<int>("search_completed_percentage_min");
        int percentageMax = Mod::get()->getSavedValue<int>("search_completed_percentage_max");

        auto coinDict = GameStatsManager::sharedState()->m_verifiedUserCoins;
        auto coinDict2 = GameStatsManager::sharedState()->m_pendingUserCoins;

        CCArray* pRet = CCArray::create();

        //uint64_t now = BetterInfo::timeInMs();

        auto levels = this->m_onlineLevels;
        CCDictElement* obj;
        CCDICT_FOREACH(levels, obj){
            auto currentLvl = static_cast<GJGameLevel*>(obj->getObject());
            switch(mode){
                case CompleteMode::completed:
                    if(currentLvl->m_normalPercent == 100) pRet->addObject(currentLvl);
                    break;
                case CompleteMode::completed21:
                    if(currentLvl->m_orbCompletion == 100) pRet->addObject(currentLvl);
                    break;
                case CompleteMode::completed211:
                    if(currentLvl->m_newNormalPercent2 == 100) pRet->addObject(currentLvl);
                    break;
                case CompleteMode::allCoins:
                case CompleteMode::noCoins: {
                    bool completed = true;
                    
                    if(currentLvl->m_normalPercent < 100) continue;

                    auto coins = currentLvl->m_coins ? currentLvl->m_coins : BICache->getCoinCount(currentLvl->m_levelID);
                    for(int i = 0; i < coins; i++){
                        auto key = currentLvl->getCoinKey(i + 1);
                        bool hasCoin = coinDict->objectForKey(key) != nullptr || coinDict2->objectForKey(key) != nullptr;
                        completed = hasCoin ? (completed && true) : false;
                    }
                    if(((mode == CompleteMode::noCoins) != completed) && (coins > 0)) pRet->addObject(currentLvl);
                    
                    break;
                }
                case CompleteMode::percentage:
                    if((percentageMin == 0 || currentLvl->m_normalPercent >= percentageMin) && (percentageMax == 0 || currentLvl->m_normalPercent <= percentageMax)) pRet->addObject(currentLvl);
                    break;
                default:
                    break;
            }
        }
        //std::cout <<  ("Unlocking shared_lock GLM::getCompletedLevels") << std::endl;
        //log::info("Took {} seconds to filter levels", BetterInfo::timeInMs() - now);

        return pRet;
    }

    CCArray* getSavedLevels(bool filter, int folderID){
        CCArray* original = GameLevelManager::getSavedLevels(filter, folderID);

        if(m_fields->skipSavedFilter || !BetterInfo::isSavedFiltered()) return original;

        if(m_fields->filteredLevels != nullptr && Mod::get()->getSavedValue<bool>("user_search_dirty", true)) {
            m_fields->filteredLevels = nullptr;
        }

        if(m_fields->filteredLevels != nullptr) return m_fields->filteredLevels;

        CCArray* pRet = CCArray::create();

        //getting config
        std::vector<unsigned int> len;
        for(unsigned int i = 0; i <= 5; i++){
            if(
                Mod::get()->getSavedValue<bool>(
                    CCString::createWithFormat("user_search_len_%02u", i)->getCString()
                )
            ) len.push_back(i);
        }

        std::vector<int> diff;

        if(Mod::get()->getSavedValue<bool>("user_search_diff_auto")) diff.push_back(-1);
        for(int i = 0; i <= 6; i++){
            if(
                Mod::get()->getSavedValue<bool>(
                    CCString::createWithFormat("user_search_diff_%02i", i)->getCString()
                )
            ) diff.push_back(i);
        }

        std::vector<int> demonDiff;

        for(int i = 0; i <= 5; i++){
            if(
                Mod::get()->getSavedValue<bool>(
                    CCString::createWithFormat("user_search_demon_%02i", i)->getCString()
                )
            ) demonDiff.push_back(i);
        }
        
        //calculating levels
        auto levels = this->m_onlineLevels;
        CCObject* obj;
        CCARRAY_FOREACH(original, obj){
            auto level = static_cast<GJGameLevel*>(obj);

            int password = level->m_password;
            int difficulty = LevelUtils::levelDifficultyAsInt(level);
            if(!(diff.empty()) && std::find(diff.begin(), diff.end(), difficulty) == diff.end()) continue;

            if(!(len.empty()) && std::find(len.begin(), len.end(), level->m_levelLength) == len.end()) continue;

            if(Mod::get()->getSavedValue<bool>("user_search_diff_06") && level->m_demon != 0) {
                int demonDifficulty = LevelUtils::levelDemonDifficultyAsInt(level);

                if(!(demonDiff.empty()) && std::find(demonDiff.begin(), demonDiff.end(), demonDifficulty) == demonDiff.end()) continue;
            }

            if(Mod::get()->getSavedValue<bool>("user_search_star") && level->m_stars == 0) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_uncompleted") && level->m_normalPercent == 100) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_completed") && level->m_normalPercent != 100) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_uncompletedorbs") && level->m_orbCompletion == 100) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_completedorbs") && level->m_orbCompletion != 100) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_uncompletedleaderboard") && level->m_newNormalPercent2 == 100) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_completedleaderboard") && level->m_newNormalPercent2 != 100) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_featured") && level->m_featured < 1) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_nofeatured") && level->m_featured >= 1) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_original") && level->m_originalLevel != 0) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_epic") && level->m_isEpic != 1) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_legendary") && level->m_isEpic != 2) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_mythic") && level->m_isEpic != 3) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_noepic") && level->m_isEpic) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_song")) {
                if(Mod::get()->getSavedValue<bool>("user_search_song_custom") && level->m_songID != Mod::get()->getSavedValue<int>("user_search_song_id")) continue;
                if(!Mod::get()->getSavedValue<bool>("user_search_song_custom") && (level->m_audioTrack != Mod::get()->getSavedValue<int>("user_search_song_id") || level->m_songID != 0)) continue;
            }
            if(Mod::get()->getSavedValue<bool>("user_search_nostar") && level->m_stars != 0) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_verifiedcoins") && (level->m_coinsVerified == 0)) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_nocoins") && level->m_coins != 0) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_twoplayer") && !(level->m_twoPlayerMode)) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_copied") && level->m_originalLevel == 0) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_downloaded") && std::string(level->m_levelString).empty()) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_ldm") && !(level->m_lowDetailMode)) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_copy") && password == 0) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_copy_free") && password != 1) continue;

            if(!validateRangeOption("user_search_idrange", level->m_levelID)) continue;
            if(!validateRangeOption("user_search_starrange", level->m_stars)) continue;
            if(!validateRangeOption("user_search_percentage", level->m_normalPercent)) continue;
            if(!validateRangeOption("user_search_percentageorbs", level->m_orbCompletion)) continue;
            if(!validateRangeOption("user_search_percentageleaderboard", level->m_newNormalPercent2)) continue;
            if(!validateRangeOption("user_search_gameversion", level->m_gameVersion)) continue;
            if(!validateRangeOption("user_search_coins", level->m_coins, 1, 3)) continue;

            bool hasAllCoins = LevelUtils::levelHasCollectedCoins(level);
            if(Mod::get()->getSavedValue<bool>("user_search_completedcoins") && (!hasAllCoins || level->m_coins == 0)) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_uncompletedcoins") && (hasAllCoins || level->m_coins == 0)) continue;

            pRet->addObject(level);
        }

        Mod::get()->setSavedValue<bool>("user_search_dirty", false);

        m_fields->filteredLevels = pRet;
        return pRet;
    }

    void limitSavedLevels(){
        m_fields->skipSavedFilter = true;

        GameLevelManager::limitSavedLevels();

        m_fields->skipSavedFilter = false;
    }

    /**
     * BetterErrors:tm:
     */
    void onProcessHttpRequestCompleted(CCHttpClient* client, CCHttpResponse* response) {
        GameLevelManager::onProcessHttpRequestCompleted(client, response);

        auto headerVector = response->getResponseHeader();
        auto header = std::string(headerVector->begin(), headerVector->end());

        auto dataVector = response->getResponseData();
        if(dataVector->size() > 11 && dataVector->at(0) == 'e') {
            auto headers = utils::string::split(header, "\n");
            for(auto& header : headers) {
                if(header.size() < 14 || !header.starts_with("Retry-After")) continue;

                auto value = header.substr(13);
                auto seconds = BetterInfo::stoi(value);

                ServerUtils::showRateLimitError(seconds);
                return;
            }

            auto data = std::string(dataVector->begin(), dataVector->end());
            ServerUtils::showCFError(data);
        }
        
    }
};