#include <Geode/Bindings.hpp>
#include <Geode/modify/GameLevelManager.hpp>

#include "../enums.hpp"
#include "../utils.hpp"

using namespace geode::prelude;

class $modify(GameLevelManager) {
    bool skipSavedFilter = false;

    /*
     * Helpers
     */

    bool validateRangeOption(const std::string& option, int value) {
        if(Mod::get()->getSavedValue<bool>(option)) {
            int min = Mod::get()->getSavedValue<int>(fmt::format("{}_min", option));
            int max = Mod::get()->getSavedValue<int>(fmt::format("{}_max", option));
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

        if(Mod::get()->getSettingValue<bool>("fix-green-users") && (userNameStd == "" || userNameStd == "-")){
            //todo: implement gdhistory functionality

            if(userID == 32471) userName = "PixelCube"; //previous dataset had an error
        }

        return userName;
    }

    CCArray* getCompletedLevels(bool newFilter){
        CompleteMode mode = static_cast<CompleteMode>(Mod::get()->getSavedValue<int>("search_completed"));
        if(mode == CompleteMode::modeDefault) return GameLevelManager::getCompletedLevels(newFilter);

        int percentageMin = Mod::get()->getSavedValue<int>("search_completed_percentage_min");
        int percentageMax = Mod::get()->getSavedValue<int>("search_completed_percentage_max");

        auto coinDict = GameStatsManager::sharedState()->m_verifiedUserCoins;
        auto coinDict2 = GameStatsManager::sharedState()->m_pendingUserCoins;

        CCArray* pRet = CCArray::create();

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
                    for(int i = 0; i < currentLvl->m_coins; i++){
                        bool hasntCoin = coinDict->objectForKey(currentLvl->getCoinKey(i + 1)) == nullptr && coinDict2->objectForKey(currentLvl->getCoinKey(i + 1)) == nullptr;
                        if(hasntCoin) completed = false; else completed = completed && true;
                    }
                    if(((mode == CompleteMode::noCoins) != completed) && (currentLvl->m_coins > 0)) pRet->addObject(currentLvl);
                    //if(currentLvl->m_coins > 0) pRet->addObject(currentLvl);
                    break;
                }
                case CompleteMode::percentage:
                    if((percentageMin == 0 || currentLvl->m_normalPercent >= percentageMin) && (percentageMax == 0 || currentLvl->m_normalPercent <= percentageMax)) pRet->addObject(currentLvl);
                    break;
            }
        }

        return pRet;
    }

    CCArray* getSavedLevels(bool filter, int folderID){
        CCArray* original = GameLevelManager::getSavedLevels(filter, folderID);

        if(m_fields->skipSavedFilter) return original;

        CCArray* pRet = CCArray::create();

        //getting config
        std::vector<unsigned int> len;
        for(unsigned int i = 0; i <= 4; i++){
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
            int difficulty = BetterInfo::levelDifficultyAsInt(level);
            if(!(diff.empty()) && std::find(diff.begin(), diff.end(), difficulty) == diff.end()) continue;

            if(!(len.empty()) && std::find(len.begin(), len.end(), level->m_levelLength) == len.end()) continue;

            if(Mod::get()->getSavedValue<bool>("user_search_diff_06") && level->m_demon != 0) {
                int demonDifficulty = BetterInfo::levelDemonDifficultyAsInt(level);

                if(!(demonDiff.empty()) && std::find(demonDiff.begin(), demonDiff.end(), demonDifficulty) == demonDiff.end()) continue;
            }

            if(Mod::get()->getSavedValue<bool>("user_search_star") && level->m_stars == 0) continue;
            //TODO: respect completed mode
            if(Mod::get()->getSavedValue<bool>("user_search_uncompleted") && level->m_normalPercent == 100) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_completed") && level->m_normalPercent != 100) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_uncompletedorbs") && level->m_orbCompletion == 100) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_completedorbs") && level->m_orbCompletion != 100) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_uncompletedleaderboard") && level->m_newNormalPercent2 == 100) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_completedleaderboard") && level->m_newNormalPercent2 != 100) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_featured") && level->m_featured < 1) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_nofeatured") && level->m_featured >= 1) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_original") && level->m_originalLevel != 0) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_epic") && !(level->m_isEpic)) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_noepic") && level->m_isEpic) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_song")) {
                if(Mod::get()->getSavedValue<bool>("user_search_song_custom") && level->m_songID != Mod::get()->getSavedValue<int>("user_search_song_id")) continue;
                if(!Mod::get()->getSavedValue<bool>("user_search_song_custom") && (level->m_audioTrack != Mod::get()->getSavedValue<int>("user_search_song_id") || level->m_songID != 0)) continue;
            }
            if(Mod::get()->getSavedValue<bool>("user_search_nostar") && level->m_stars != 0) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_coins") && (level->m_coins == 0 || level->m_coinsVerified == 0)) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_nocoins") && (level->m_coins != 0)) continue;
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

            bool hasAllCoins = BetterInfo::levelHasCollectedCoins(level);
            if(Mod::get()->getSavedValue<bool>("user_search_completedcoins") && (!hasAllCoins || level->m_coins == 0)) continue;
            if(Mod::get()->getSavedValue<bool>("user_search_uncompletedcoins") && (hasAllCoins || level->m_coins == 0)) continue;

            pRet->addObject(level);
        }

        return pRet;
    }

    void limitSavedLevels(){
        m_fields->skipSavedFilter = true;

        GameLevelManager::limitSavedLevels();

        m_fields->skipSavedFilter = false;
    }
};