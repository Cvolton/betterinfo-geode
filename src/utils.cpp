#include "utils.hpp"

#include <random>
#include <fmt/format.h>
#include <time.h>
#include <chrono>

#include <Geode/cocos/support/base64.h>
#include <Geode/cocos/support/zip_support/ZipUtils.h>
#include <Geode/utils/web.hpp>

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
    return searchObject->m_searchType == SearchType::Type19
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
    || searchObject->m_searchType == SearchType::DailySafe
    || searchObject->m_searchType == SearchType::WeeklySafe
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
    return ZipUtils::decompressString(input, false, 0);
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

void BetterInfo::copyToClipboard(const char* text){
    clipboard::write(text);
}

void BetterInfo::copyToClipboard(const char* text, CCLayer* parent){
    copyToClipboard(text);

    Notification::create("Copied to clipboard", NotificationIcon::None)->show();
    //parent->addChild(TextAlertPopup::create("Copied to clipboard", 0.5f, 0.6f), 100);
}

std::string BetterInfo::getSongUrl(int audioID) {
    //TODO: reverse MusicDownloadManager
    return "";
    /*auto songInfo = static_cast<SongInfoObject*>(MusicDownloadManager::sharedState()->m_songsDict->objectForKey(std::to_string(audioID)));
    if(!songInfo) return "";
    return songInfo->m_songURL;*/
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

bool BetterInfo::validateRangeItem(const BISearchObject::RangeItem& rangeItem, int value) {
    if(!rangeItem.enabled) return true;
    if(rangeItem.min != 0 && rangeItem.min > value) return false;
    if(rangeItem.max != 0 && rangeItem.max < value) return false;
    return true;
}

bool BetterInfo::levelMatchesObject(GJGameLevel* level, const BISearchObject& searchObj) {

    if(!searchObj.difficulty.empty() && searchObj.difficulty.find(LevelUtils::levelDifficultyAsInt(level)) == searchObj.difficulty.end()) return false;
    if(!searchObj.length.empty() && searchObj.length.find(level->m_levelLength) == searchObj.length.end()) return false;
    if(!searchObj.demonDifficulty.empty() && level->m_demon != 0
        && searchObj.difficulty.find(6) != searchObj.difficulty.end()
        && searchObj.demonDifficulty.find(LevelUtils::levelDemonDifficultyAsInt(level)) == searchObj.demonDifficulty.end()
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

    bool hasAllCoins = LevelUtils::levelHasCollectedCoins(level);
    if(searchObj.completedCoins && (!hasAllCoins || level->m_coins == 0)) return false;
    if(searchObj.uncompletedCoins && (hasAllCoins || level->m_coins == 0)) return false;

    return true;
}

bool BetterInfo::levelProgressMatchesObject(GJGameLevel* level, const BISearchObject& searchObj) {
    if(!validateRangeItem(searchObj.idRange, level->m_levelID)) return false;

    auto levelFromSaved = LevelUtils::getLevelFromSaved(level->m_levelID);
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
    //TODO: reverse LevelCell

    /*auto listLayer = getChildOfType<GJListLayer>(levelBrowserLayer, 0);
    if(!listLayer) return;
    auto listView = getChildOfType<CustomListView>(listLayer, 0);
    if(!listView) return;
    auto tableView = getChildOfType<TableView>(listView, 0);
    if(!tableView) return;
    auto contentLayer = getChildOfType<CCContentLayer>(tableView, 0);
    if(!contentLayer) return;
    auto children = CCArrayExt<CCNode>(contentLayer->getChildren());

    for(auto& child : children) {
        auto levelCell = typeinfo_cast<LevelCell*>(child);
        if(levelCell) {
            auto layer = static_cast<CCLayer*>(levelCell->getChildren()->objectAtIndex(1));
            auto menu = getChildOfType<CCMenu>(layer, 0);
            auto playerName = static_cast<CCMenuItemSpriteExtra*>(menu->getChildren()->objectAtIndex(1));
            auto textNode = static_cast<CCLabelBMFont*>(playerName->getChildren()->objectAtIndex(0));

            float oldXSize = textNode->getScaledContentSize().width;

            auto userName = GameLevelManager::sharedState()->userNameForUserID(levelCell->m_level->m_userID);

            auto oldString = std::string(textNode->getString());
            auto newString = fmt::format("By {}", std::string(userName));

            textNode->setString(newString.c_str());
            playerName->setContentSize(textNode->getContentSize() * textNode->getScale());
            auto winSize = CCDirector::sharedDirector()->getWinSize();
            playerName->setPositionX((-285.5 + 23.5) + (playerName->getContentSize().width / 2));
            textNode->setPositionX(playerName->getContentSize().width / 2);

            float difference = textNode->getScaledContentSize().width - oldXSize;

            for(unsigned int i = 0; i < layer->getChildrenCount(); i++){
                CCSprite* node = typeinfo_cast<CCSprite*>(layer->getChildren()->objectAtIndex(i));
                if(node == nullptr) continue;

                if(isSprite(node, "collaborationIcon_001.png") || isSprite(node, "highObjectIcon_001.png")) {
                    
                    node->setPositionX(node->getPositionX() + difference);

                }
            }

            levelCell->m_level->m_creatorName = userName;

        }
    }*/
}

inline bool objectIDIsSpeedPortal(int id) {
    return (id == 200 || id == 201 || id == 202 || id == 203 || id == 1334);
}

inline float travelForSpeed(int speed) {
    switch (speed)
    {
    case 1:
        return 251.16008f;
        break;
    case 2:
        return 387.42014f;
        break;
    case 3:
        return 468.00015f;
        break;
    case 4:
        return 576.00018f;
        break;
    default:
        return 311.58011f;
        break;
    }
}

inline float travelForPortalId(int speed) {
    switch (speed)
    {
    case 200:
        return 251.16008f;
        break;
    default:
        return 311.58011f;
        break;
    case 202:
        return 387.42014f;
        break;
    case 203:
        return 468.00015f;
        break;
    case 1334:
        return 576.00018f;
        break;
    }
}

inline int speedToPortalId(int speed) {
    switch(speed) {
    case 3:
        return 202;
        break;
    case 4:
        return 1334;
        break;
    case 1:
        return 200;
        break;
    default:
        return 201;
        break;
    case 2:
        return 203;
        break;
    }
}

inline uint64_t timeInMs() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

float BetterInfo::timeForLevelString(const std::string& levelString) {
        std::string epicString;
    try {
        //todo: checked portals
        auto a = timeInMs();

        auto decompressString = decodeBase64Gzip(levelString);
        auto c = timeInMs();
        std::stringstream responseStream(decompressString);
        std::string currentObject;
        std::string currentKey;
        std::string keyID;

        //std::stringstream objectStream;
        float prevPortalX = 0;
        int prevPortalId = 0;

        float timeFull = 0;

        float maxPos = 0;
        while(getline(responseStream, currentObject, ';')){
            size_t i = 0;
            int objID = 0;
            float xPos = 0;

            /*objectStream.str("");
            objectStream.clear();
            objectStream << currentObject;
            objectStream.seekp(0);
            objectStream.seekg(0);*/
            std::stringstream objectStream(currentObject);
            while(getline(objectStream, currentKey, ',')) {
                epicString += currentKey + "\n";

                if(i % 2 == 0) keyID = currentKey;
                else {
                    if(keyID == "1") objID = std::stoi(currentKey);
                    else if(keyID == "2") xPos = std::stof(currentKey);
                    else if(keyID == "kA4") prevPortalId = speedToPortalId(std::stoi(currentKey));
                }
                i++;

                if(xPos != 0 && objID != 0) break;
            }

            if(maxPos < xPos) maxPos = xPos;
            if(!objectIDIsSpeedPortal(objID)) continue;

            timeFull += (xPos - prevPortalX) / travelForPortalId(prevPortalId);
            prevPortalId = objID;
            prevPortalX = xPos;
        }

        timeFull += (maxPos - prevPortalX) / travelForPortalId(prevPortalId);
        auto b = timeInMs() - a;
        log::debug("time spent decompressing: {}", c - a);
        log::debug("time spent: {}", b);
        return timeFull;
    } catch(std::exception e) {
        log::error("An exception has occured while calculating time for levelString: {}", e.what());
        return 0;
    }
}

bool BetterInfo::controllerConnected() {
    #ifdef GEODE_IS_ANDROID
    return false;
    #else
    return CCApplication::sharedApplication()->getControllerConnected();
    #endif
}

void BetterInfo::loadImportantNotices(CCLayer* layer) {
    static bool hasBeenCalled = false;
    if(hasBeenCalled) return;
    hasBeenCalled = true;

    layer->retain();

    web::AsyncWebRequest().fetch(fmt::format("https://geometrydash.eu/mods/betterinfo/_api/importantNotices/?platform={}&version={}", GEODE_PLATFORM_NAME, Mod::get()->getVersion().toString(false))).json().then([layer](const matjson::Value& info){
        auto notice = info.try_get("notice");
        if(notice == std::nullopt) return;
        
        if(info["notice"].is_string()) {
            auto alert = FLAlertLayer::create("BetterInfo", info["notice"].as_string(), "OK");
            alert->m_scene = layer;
            alert->show();
            layer->release();
        }
    }).expect([](const std::string& error){
        log::warn("Fetching important notices failed: {}", error);
    });
}

//from coloride on geode sdk discord
bool BetterInfo::isHoveringNode(CCNode* target) {
    CCPoint touchPos = getMousePos();

    CCPoint targetWorldPos = target->getParent()->convertToWorldSpace(target->getPosition());
    auto targetSize = target->getContentSize();
    auto targetScale = target->getScale();
    auto anchorPoint = target->getAnchorPoint();

    CCRect bounds = CCRect(
        targetWorldPos.x,// - targetSize.width * targetScale * anchorPoint.x,
        targetWorldPos.y,// - targetSize.height * targetScale * anchorPoint.y,
        targetSize.width * targetScale,
        targetSize.height * targetScale
    );

    return bounds.containsPoint(touchPos);
}

bool BetterInfo::isSprite(CCSprite* sprite, const char* name) {
    auto cache = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(name);
    if(!cache) return false;

    return cache->getTexture() == sprite->getTexture() && cache->getRect() == sprite->getTextureRect();
}