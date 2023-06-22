#include "utils.hpp"

#include <random>
#include <fmt/format.h>

#include <Geode/cocos/support/base64.h>
#include <Geode/cocos/support/zip_support/ZipUtils.h>

CCSprite* BetterInfo::createWithBISpriteFrameName(const char* name){
        return createBISprite(name);
}

CCSprite* BetterInfo::createBISprite(const char* name){
        name = Mod::get()->expandSpriteName(name);
        auto sprite = CCSprite::create(name);
        if(sprite) return sprite;

        return createPlaceholder();
}

CCSprite* BetterInfo::createPlaceholder(){
        auto sprite = CCSprite::create();

        auto subSprite1 = CCSprite::create("square02b_001.png");
        auto subSprite2 = CCSprite::create("square02b_001.png");
        auto subSprite3 = CCSprite::create("square02b_001.png");
        auto subSprite4 = CCSprite::create("square02b_001.png");

        std::array<CCSprite*, 4> sprites = {subSprite1, subSprite2, subSprite3, subSprite4};

        subSprite1->setAnchorPoint({0,0});
        subSprite2->setAnchorPoint({1,0});
        subSprite3->setAnchorPoint({0,1});
        subSprite4->setAnchorPoint({1,1});

        subSprite2->setColor({251,64,249});
        subSprite1->setColor({0,0,0});
        subSprite3->setColor({251,64,249});
        subSprite4->setColor({0,0,0});

        for(auto subSprite : sprites){
                subSprite->setScale(.25f);
                subSprite->setPosition(subSprite->getContentSize() / 2);
                sprite->addChild(subSprite);

        }

        sprite->setContentSize(subSprite1->getContentSize());

        return sprite;
}

CCMenuItemSpriteExtra* BetterInfo::createTextButton(CCLayer* parent, const char* text, cocos2d::SEL_MenuHandler handler, int width, float height, float scale){
    auto buttonSprite = ButtonSprite::create(text, width, true, "bigFont.fnt", "GJ_button_01.png", height, scale);
    auto buttonButton = CCMenuItemSpriteExtra::create(
        buttonSprite,
        parent,
        handler
    );
    buttonButton->setSizeMult(1.2f);

    return buttonButton;
}

int BetterInfo::randomNumber(int start, int end){
    std::random_device os_seed;
    const unsigned int seed = os_seed();

    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> distribute(start, end);

    return distribute(generator);
}

void BetterInfo::strToLower(std::string& str) {
        for(auto& c : str) c = tolower(c);
}

const char* BetterInfo::rankIcon(int position){
        if (position == 1) return "rankIcon_1_001.png";
        else if (position > 1000 || position <= 0) return "rankIcon_all_001.png";
        else if (position <= 10) return "rankIcon_top10_001.png";
        else if (position <= 50) return "rankIcon_top50_001.png";
        else if (position <= 100) return "rankIcon_top100_001.png";
        else if (position <= 200) return "rankIcon_top200_001.png";
        else if (position <= 500) return "rankIcon_top500_001.png";
        return "rankIcon_top1000_001.png";
}

int BetterInfo::levelsPerPage(GJSearchObject* searchObject){
        const int levelsPerPageLow = 10;
        const int levelsPerPageHigh = 20;

        return ((isLocal(searchObject) && GameManager::sharedState()->getGameVariable("0093")) ? levelsPerPageHigh : levelsPerPageLow);
}

bool BetterInfo::isLocal(GJSearchObject* searchObject){
        return searchObject->m_searchType == SearchType::MyLevels 
                || searchObject->m_searchType == SearchType::SavedLevels 
                || searchObject->m_searchType == SearchType::FavouriteLevels;
}

bool BetterInfo::isFalseTotal(GJSearchObject* searchObject){
        return searchObject->m_searchType == SearchType::ListsOnClick
                || searchObject->m_searchType == SearchType::Featured
                || searchObject->m_searchType == SearchType::HallOfFame;
}

bool BetterInfo::isStarUseless(GJSearchObject* searchObject){
        return searchObject->m_searchType == SearchType::Featured
                || searchObject->m_searchType == SearchType::Magic
                || searchObject->m_searchType == SearchType::MapPack
                || searchObject->m_searchType == SearchType::MapPackOnClick
                || searchObject->m_searchType == SearchType::Awarded
                || searchObject->m_searchType == SearchType::Users
                || searchObject->m_searchType == SearchType::HallOfFame
                || searchObject->m_searchType == SearchType::FeaturedGDW
                || searchObject->m_searchType == SearchType::Similar
                || searchObject->m_searchType == SearchType::DailyVault
                || searchObject->m_searchType == SearchType::WeeklyVault
                || isLocal(searchObject);
}

bool BetterInfo::isAdvancedEnabled(GJSearchObject* searchObject) {
        return searchObject->m_searchType == SearchType::Search
                || searchObject->m_searchType == SearchType::Downloaded
                || searchObject->m_searchType == SearchType::MostLiked
                || searchObject->m_searchType == SearchType::Trending
                || searchObject->m_searchType == SearchType::Recent
                || searchObject->m_searchType == SearchType::Awarded
                || searchObject->m_searchType == SearchType::Followed
                || searchObject->m_searchType == SearchType::Friends;
}


bool BetterInfo::isSavedFiltered() {
        for(unsigned int i = 0; i <= 4; i++){
                if(
                        Mod::get()->getSavedValue<bool>(
                                CCString::createWithFormat("user_search_len_%02u", i)->getCString()
                        )
                ) return true;
        }

        if(Mod::get()->getSavedValue<bool>("user_search_diff_auto")) return true;

        for(int i = 0; i <= 6; i++){
                if(
                        Mod::get()->getSavedValue<bool>(
                                CCString::createWithFormat("user_search_diff_%02i", i)->getCString()
                        )
                ) return true;
        }

        if(Mod::get()->getSavedValue<bool>("user_search_diff_06"))
                for(int i = 0; i <= 5; i++){
                        if(
                                Mod::get()->getSavedValue<bool>(
                                        CCString::createWithFormat("user_search_demon_%02i", i)->getCString()
                                )
                        ) return true;
                }

        const char* options[] = {
                "user_search_star", "user_search_uncompleted", "user_search_completed",
                "user_search_featured", "user_search_nofeatured", "user_search_original",
                "user_search_epic", "user_search_noepic", "user_search_song",
                "user_search_nostar", "user_search_coins", "user_search_nocoins", "user_search_twoplayer",
                "user_search_copied", "user_search_downloaded", "user_search_ldm",
                "user_search_copy", "user_search_copy_free", "user_search_idrange",
                "user_search_completedorbs", "user_search_completedleaderboard", "user_search_uncompletedorbs",
                "user_search_uncompletedleaderboard", "user_search_percentage", "user_search_percentageorbs",
                "user_search_percentageleaderboard", "user_search_starrange", "user_search_completedcoins",
                "user_search_uncompletedcoins"
        };

        for(auto option : options) {
                if(Mod::get()->getSavedValue<bool>(option)) return true;
        }

        return false;
}

std::string BetterInfo::decodeBase64Gzip(const std::string& input) {
        unsigned char* levelString;
        unsigned char* levelStringFull;
        int levelStringSize = base64Decode((unsigned char *)(input.c_str()), input.size(), &levelString);
        int levelStringSizeDeflated = ZipUtils::ccInflateMemory(levelString, levelStringSize, &levelStringFull);

        std::string levelStringFullStd((char*)levelStringFull, levelStringSizeDeflated);

        delete levelString;
        delete levelStringFull;

        return levelStringFullStd;
}

std::string BetterInfo::fileSize(size_t bytes) {
        std::stringstream size;
        size << std::setprecision(4);

        if(bytes > (1024*1024)) size << ( bytes / (float)(1024*1024) ) << "MB";
        else if(bytes > (1024)) size << ( bytes / (float)(1024) ) << "KB";
        else size << bytes << "B";

        return size.str();
}

std::string BetterInfo::fixColorCrashes(std::string input) {
        int tags = 0;

        std::string openingTag = "<c";
        for(std::string::size_type pos = 0; (pos = input.find(openingTag, pos)) != std::string::npos; tags++) {
                pos += openingTag.length();
        }

        std::string closingTag = "</c>";
        for(std::string::size_type pos = 0; (pos = input.find(closingTag, pos)) != std::string::npos; tags--) {
                pos += closingTag.length();
        }

        for(int i = 0; i < tags; i++) {
                input.append("  </c>");
        }

        return input;
}

std::string BetterInfo::fixNullByteCrash(std::string input) {
        for(auto& character : input) {
                if(character == '\0') character = ' ';
        }
        return input;
}

std::string BetterInfo::timeToString(time_t input) {
        struct tm time3;
        localtime_s(&time3, &input);
        return fmt::format("{}-{:02}-{:02} {:02}:{:02}", time3.tm_year + 1900, time3.tm_mon + 1, time3.tm_mday, time3.tm_hour, time3.tm_min);
}

void BetterInfo::copyToClipboard(const char* text){
        clipboard::write(text);
}

void BetterInfo::copyToClipboard(const char* text, CCLayer* parent){
        copyToClipboard(text);

        parent->addChild(TextAlertPopup::create("Copied to clipboard", 0.5f, 0.6f), 100);
}

std::string BetterInfo::getSongUrl(int audioID) {
        auto songInfo = static_cast<SongInfoObject*>(MusicDownloadManager::sharedState()->m_songsDict->objectForKey(std::to_string(audioID)));
        if(!songInfo) return "";
        return songInfo->m_songURL;
}

bool BetterInfo::isNewGrounds(int audioID) {
        return BetterInfo::getSongUrl(audioID).find("://audio.ngfiles.com/") != std::string::npos;
}

/*
    This is a reimplementation of GameLevelManager::responseToDict
    because I couldn't get it to work. It's not 1:1 with the original
    but it achieves the same purpose.
*/
CCDictionary* BetterInfo::responseToDict(const std::string& response){
    auto dict = CCDictionary::create();

    std::stringstream responseStream(response);
    std::string currentKey;
    std::string keyID;

    unsigned int i = 0;
    while(getline(responseStream, currentKey, ':')){

        if(i % 2 == 0) keyID = currentKey;
        else dict->setObject(CCString::create(currentKey.c_str()),keyID);

        i++;
    }

    return dict;
}

GJGameLevel* BetterInfo::getLevelFromSaved(int levelID) {
        return static_cast<GJGameLevel*>(GameLevelManager::sharedState()->m_onlineLevels->objectForKey(std::to_string(levelID)));
}

std::deque<GJGameLevel*> BetterInfo::completedDeque() {
        std::deque<GJGameLevel*> levelsDeque;
        auto levels = GameLevelManager::sharedState()->m_onlineLevels;
        CCDictElement* obj;
        CCDICT_FOREACH(levels, obj){
                auto currentLvl = static_cast<GJGameLevel*>(obj->getObject());
                levelsDeque.push_back(currentLvl);
        }
        return levelsDeque;
}

int BetterInfo::levelDifficultyAsInt(GJGameLevel* level) {
        if(level->m_demon != 0) return 6;
        if(level->m_autoLevel) return -1;
        return (level->m_ratings == 0) ? 0 : (level->m_ratingsSum / level->m_ratings);
}

int BetterInfo::levelDemonDifficultyAsInt(GJGameLevel* level) {
        int demonDifficulty = 2;
        if(level->m_demonDifficulty >= 5) demonDifficulty = level->m_demonDifficulty - 2;
        else if(level->m_demonDifficulty >= 3) demonDifficulty = level->m_demonDifficulty - 3;
        return demonDifficulty;
}

bool BetterInfo::levelHasCollectedCoins(GJGameLevel* level) {
        auto coinDict = GameStatsManager::sharedState()->m_verifiedUserCoins;
        auto coinDict2 = GameStatsManager::sharedState()->m_pendingUserCoins;
        bool hasAllCoins = true;
        for(int i = 0; i < level->m_coins; i++){
                bool hasntCoin = coinDict->objectForKey(level->getCoinKey(i + 1)) == nullptr && coinDict2->objectForKey(level->getCoinKey(i + 1)) == nullptr;
                if(hasntCoin) hasAllCoins = false;
        }
        return hasAllCoins;
}

bool BetterInfo::validateRangeItem(const BISearchObject::RangeItem& rangeItem, int value) {
        if(!rangeItem.enabled) return true;
        if(rangeItem.min != 0 && rangeItem.min > value) return false;
        if(rangeItem.max != 0 && rangeItem.max < value) return false;
        return true;
}

bool BetterInfo::levelMatchesObject(GJGameLevel* level, const BISearchObject& searchObj) {

        if(!searchObj.difficulty.empty() && searchObj.difficulty.find(levelDifficultyAsInt(level)) == searchObj.difficulty.end()) return false;
        if(!searchObj.length.empty() && searchObj.length.find(level->m_levelLength) == searchObj.length.end()) return false;
        if(!searchObj.demonDifficulty.empty() && level->m_demon != 0
                && searchObj.difficulty.find(6) != searchObj.difficulty.end()
                && searchObj.demonDifficulty.find(levelDemonDifficultyAsInt(level)) == searchObj.demonDifficulty.end()
                ) return false;

        std::string query = searchObj.str;
        std::string name = level->m_levelName;
        strToLower(query);
        strToLower(name);

        if(name.find(query) == std::string::npos) return false;


        if(searchObj.star && level->m_stars == 0) return false;
        if(searchObj.noStar && level->m_stars != 0) return false;
        if(searchObj.featured && level->m_featured <= 0) return false;
        if(searchObj.original && level->m_originalLevel > 0) return false;
        if(searchObj.twoPlayer && !level->m_twoPlayerMode) return false;
        if(searchObj.coins && (level->m_coins == 0 || level->m_coinsVerified == 0)) return false;
        if(searchObj.noCoins && (level->m_coins != 0 && level->m_coinsVerified != 0)) return false;
        if(searchObj.epic && !level->m_isEpic) return false;
        //TODO: searchObj.folder
        if(searchObj.song) {
                if(!searchObj.songCustom && level->m_audioTrack != searchObj.songID) return false;
                if(searchObj.songCustom && level->m_songID != searchObj.songID) return false;
        }
        if(searchObj.copied && level->m_originalLevel <= 0) return false;
        //TODO: searchObj.ldm
        
        //TODO: searchObj.copyable
        //TODO: searchObj.freeCopy
        if(searchObj.unfeatured && level->m_featured > 0) return false;
        if(searchObj.unepic && level->m_isEpic) return false;
        if(!validateRangeItem(searchObj.starRange, level->m_stars)) return false;
        if(!validateRangeItem(searchObj.gameVersion, level->m_gameVersion)) return false;

        if(!levelProgressMatchesObject(level, searchObj)) return false;

        bool hasAllCoins = levelHasCollectedCoins(level);
        if(searchObj.completedCoins && (!hasAllCoins || level->m_coins == 0)) return false;
        if(searchObj.uncompletedCoins && (hasAllCoins || level->m_coins == 0)) return false;

        return true;
}

bool BetterInfo::levelProgressMatchesObject(GJGameLevel* level, const BISearchObject& searchObj) {
        if(!validateRangeItem(searchObj.idRange, level->m_levelID)) return false;

        auto levelFromSaved = getLevelFromSaved(level->m_levelID);
        if(searchObj.uncompleted && (levelFromSaved && levelFromSaved->m_normalPercent == 100)) return false;
        if(searchObj.uncompletedOrbs && (!levelFromSaved || levelFromSaved->m_orbCompletion == 100)) return false;
        if(searchObj.uncompletedLeaderboard && (!levelFromSaved || levelFromSaved->m_newNormalPercent2 == 100)) return false;

        if(searchObj.completed && (!levelFromSaved || levelFromSaved->m_normalPercent != 100)) return false;
        if(searchObj.completedOrbs && (!levelFromSaved || levelFromSaved->m_orbCompletion != 100)) return false;
        if(searchObj.completedLeaderboard && (!levelFromSaved || levelFromSaved->m_newNormalPercent2 != 100)) return false;

        if(!validateRangeItem(searchObj.percentage, (levelFromSaved ? levelFromSaved->m_normalPercent : 0))) return false;
        if(!validateRangeItem(searchObj.percentageOrbs, (levelFromSaved ? levelFromSaved->m_orbCompletion : 0))) return false;
        if(!validateRangeItem(searchObj.percentageLeaderboard, (levelFromSaved ? levelFromSaved->m_newNormalPercent2 : 0))) return false;

        if(searchObj.downloaded && (!levelFromSaved || std::string(levelFromSaved->m_levelString).empty())) return false;

        return true;
}

void BetterInfo::reloadUsernames(LevelBrowserLayer* levelBrowserLayer) {
        //auto newLayer 

        auto listLayer = getChildOfType<GJListLayer>(levelBrowserLayer, 0);
        auto listView = getChildOfType<CustomListView>(listLayer, 0);
        auto tableView = getChildOfType<TableView>(listView, 0);
        auto contentLayer = getChildOfType<CCContentLayer>(tableView, 0);
        auto children = CCArrayExt<CCNode>(contentLayer->getChildren());

        for(auto& child : children) {
                auto levelCell = typeinfo_cast<LevelCell*>(child);
                if(levelCell) {
                        auto layer = static_cast<CCLayer*>(levelCell->getChildren()->objectAtIndex(1));
                        auto menu = getChildOfType<CCMenu>(layer, 0);
                        auto playerName = static_cast<CCMenuItemSpriteExtra*>(menu->getChildren()->objectAtIndex(1));
                        auto textNode = static_cast<CCLabelBMFont*>(playerName->getChildren()->objectAtIndex(0));

                        auto oldString = std::string(textNode->getString());
                        auto newString = fmt::format("By {}", std::string(GameLevelManager::sharedState()->userNameForUserID(levelCell->m_level->m_userID)));

                        size_t difference = newString.length() - oldString.length();

                        textNode->setString(newString.c_str());
                        playerName->setPositionX(playerName->getPositionX() + (difference * 5));

                }
        }
}