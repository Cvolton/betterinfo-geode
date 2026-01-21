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

    inline bool validateRangeOption(const BISearchObject::RangeItem& option, int value, int defaultMin = 0, int defaultMax = 0) {
        if(!option.enabled) return true;
        if(option.min != 0 && option.min > value) return false;
        if(option.max != 0 && option.max < value) return false;
        return true;
    }

    BISearchObject::RangeItem createRangeOption(const std::string& option) {
        BISearchObject::RangeItem rangeItem;
        rangeItem.enabled = Mod::get()->getSavedValue<bool>(option);
        rangeItem.min = Mod::get()->getSavedValue<int>(fmt::format("{}_min", option), 0);
        rangeItem.max = Mod::get()->getSavedValue<int>(fmt::format("{}_max", option), 0);
        return rangeItem;
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

        for(auto [key, currentLvl] : CCDictionaryExt<gd::string, GJGameLevel>(this->m_onlineLevels)) {
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

        if(m_fields->filteredLevels != nullptr && folderID == 0) return m_fields->filteredLevels;

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

        BISearchObject searchObj {
            .star = Mod::get()->getSavedValue<bool>("user_search_star"),
            .noStar = Mod::get()->getSavedValue<bool>("user_search_nostar"),
            .uncompleted = Mod::get()->getSavedValue<bool>("user_search_uncompleted"),
            .uncompletedOrbs = Mod::get()->getSavedValue<bool>("user_search_uncompletedorbs"),
            .uncompletedLeaderboard = Mod::get()->getSavedValue<bool>("user_search_uncompletedleaderboard"),
            .uncompletedCoins = Mod::get()->getSavedValue<bool>("user_search_uncompletedcoins"),
            .completed = Mod::get()->getSavedValue<bool>("user_search_completed"),
            .completedOrbs = Mod::get()->getSavedValue<bool>("user_search_completedorbs"),
            .completedLeaderboard = Mod::get()->getSavedValue<bool>("user_search_completedleaderboard"),
            .completedCoins = Mod::get()->getSavedValue<bool>("user_search_completedcoins"),
            .percentage = createRangeOption("user_search_percentage"),
            .percentageOrbs = createRangeOption("user_search_percentageorbs"),
            .percentageLeaderboard = createRangeOption("user_search_percentageleaderboard"),
            .featured = Mod::get()->getSavedValue<bool>("user_search_featured"),
            .original = Mod::get()->getSavedValue<bool>("user_search_original"),
            .twoPlayer = Mod::get()->getSavedValue<bool>("user_search_twoplayer"),
            .coins = createRangeOption("user_search_coins"),
            .noCoins = Mod::get()->getSavedValue<bool>("user_search_nocoins"),
            .verifiedCoins = Mod::get()->getSavedValue<bool>("user_search_verifiedcoins"),
            .unverifiedCoins = Mod::get()->getSavedValue<bool>("user_search_unverifiedcoins"),
            .epic = Mod::get()->getSavedValue<bool>("user_search_epic"),
            .legendary = Mod::get()->getSavedValue<bool>("user_search_legendary"),
            .mythic = Mod::get()->getSavedValue<bool>("user_search_mythic"),
            .song = Mod::get()->getSavedValue<bool>("user_search_song"),
            .songCustom = Mod::get()->getSavedValue<bool>("user_search_song_custom"),
            .songID = Mod::get()->getSavedValue<int>("user_search_song_id"),
            .copied = Mod::get()->getSavedValue<bool>("user_search_copied"),
            .downloaded = Mod::get()->getSavedValue<bool>("user_search_downloaded"),
            .ldm = Mod::get()->getSavedValue<bool>("user_search_ldm"),
            .idRange = createRangeOption("user_search_idrange"),
            .copyable = Mod::get()->getSavedValue<bool>("user_search_copy"),
            .freeCopy = Mod::get()->getSavedValue<bool>("user_search_copy_free"),
            .unfeatured = Mod::get()->getSavedValue<bool>("user_search_nofeatured"),
            .unepic = Mod::get()->getSavedValue<bool>("user_search_noepic"),
            .starRange = createRangeOption("user_search_starrange"),
            .gameVersion = createRangeOption("user_search_gameversion"),
        };
        
        //calculating levels
        for(auto [key, level] : CCDictionaryExt<gd::string, GJGameLevel>(this->m_onlineLevels)){

            int password = level->m_password;
            int difficulty = LevelUtils::levelDifficultyAsInt(level);
            if(!(diff.empty()) && std::find(diff.begin(), diff.end(), difficulty) == diff.end()) continue;

            if(!(len.empty()) && std::find(len.begin(), len.end(), level->m_levelLength) == len.end()) continue;

            if(std::find(diff.begin(), diff.end(), 6) != diff.end() && level->m_demon != 0) {
                int demonDifficulty = LevelUtils::levelDemonDifficultyAsInt(level);

                if(!(demonDiff.empty()) && std::find(demonDiff.begin(), demonDiff.end(), demonDifficulty) == demonDiff.end()) continue;
            }

            if(searchObj.star && level->m_stars == 0) continue;
            if(searchObj.uncompleted && level->m_normalPercent == 100) continue;
            if(searchObj.completed && level->m_normalPercent != 100) continue;
            if(searchObj.uncompletedOrbs && level->m_orbCompletion == 100) continue;
            if(searchObj.completedOrbs && level->m_orbCompletion != 100) continue;
            if(searchObj.uncompletedLeaderboard && level->m_newNormalPercent2 == 100) continue;
            if(searchObj.completedLeaderboard && level->m_newNormalPercent2 != 100) continue;
            if(searchObj.featured && level->m_featured < 1) continue;
            if(searchObj.unfeatured && level->m_featured >= 1) continue;
            if(searchObj.original && level->m_originalLevel != 0) continue;
            if(searchObj.epic && level->m_isEpic != 1) continue;
            if(searchObj.legendary && level->m_isEpic != 2) continue;
            if(searchObj.mythic && level->m_isEpic != 3) continue;
            if(searchObj.unepic && level->m_isEpic) continue;
            if(searchObj.song) {
                if(searchObj.songCustom && level->m_songID != searchObj.songID) continue;
                if(!searchObj.songCustom && (level->m_songID != 0 || level->m_audioTrack != searchObj.songID)) continue;
            }
            if(searchObj.noStar && level->m_stars != 0) continue;
            if(searchObj.verifiedCoins && (level->m_coinsVerified == 0)) continue;
            if(searchObj.unverifiedCoins && (level->m_coinsVerified)) continue;
            if(searchObj.noCoins && level->m_coins != 0) continue;
            if(searchObj.twoPlayer && !(level->m_twoPlayerMode)) continue;
            if(searchObj.copied && level->m_originalLevel == 0) continue;
            if(searchObj.downloaded && level->m_levelString.empty()) continue;
            if(searchObj.ldm && !(level->m_lowDetailMode)) continue;
            if(searchObj.copyable && password == 0) continue;
            if(searchObj.freeCopy && password != 1) continue;

            if(!validateRangeOption(searchObj.idRange, level->m_levelID)) continue;
            if(!validateRangeOption(searchObj.starRange, level->m_stars)) continue;
            if(!validateRangeOption(searchObj.percentage, level->m_normalPercent)) continue;
            if(!validateRangeOption(searchObj.percentageOrbs, level->m_orbCompletion)) continue;
            if(!validateRangeOption(searchObj.percentageLeaderboard, level->m_newNormalPercent2)) continue;
            if(!validateRangeOption(searchObj.gameVersion, level->m_gameVersion)) continue;
            if(!validateRangeOption(searchObj.coins, level->m_coins, 1, 3)) continue;

            if(searchObj.completedCoins || searchObj.uncompletedCoins) {
                bool hasAllCoins = LevelUtils::levelHasCollectedCoins(level, true);
                if(searchObj.completedCoins && (!hasAllCoins || level->m_coins == 0)) continue;
                if(searchObj.uncompletedCoins && (hasAllCoins || level->m_coins == 0)) continue;
            }

            pRet->addObject(level);
        }

        if(folderID == 0) {
            Mod::get()->setSavedValue<bool>("user_search_dirty", false);
            m_fields->filteredLevels = pRet;
        }

        return pRet;
    }

    void limitSavedLevels(){
        m_fields->skipSavedFilter = true;

        GameLevelManager::limitSavedLevels();

        m_fields->skipSavedFilter = false;
    }

    void deleteLevel(GJGameLevel* level) {
        Mod::get()->setSavedValue<bool>("user_search_dirty", true);

        GameLevelManager::deleteLevel(level);
    }

    /**
     * BetterErrors:tm:
     */
    void onProcessHttpRequestCompleted(CCHttpClient* client, CCHttpResponse* response) {
        GameLevelManager::onProcessHttpRequestCompleted(client, response);

        auto headerVector = response->getResponseHeader();
        auto header = std::string(headerVector->data(), headerVector->size());

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

            auto data = std::string(dataVector->data(), dataVector->size());
            ServerUtils::showCFError(data);
        }
        
    }
};