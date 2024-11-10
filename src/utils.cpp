#include "utils.hpp"
#include "Geode/Enums.hpp"
#include "Geode/utils/VersionInfo.hpp"

#include <random>
#include <fmt/format.h>
#include <time.h>
#include <chrono>
#include <charconv>

#include <Geode/Geode.hpp>
#include <Geode/cocos/support/base64.h>
#include <Geode/cocos/support/zip_support/ZipUtils.h>
#include <Geode/utils/web.hpp>
#include <Geode/ui/GeodeUI.hpp>

// https://stackoverflow.com/questions/25986331/how-to-determine-windows-version-in-future-proof-way
#ifdef GEODE_IS_WINDOWS
#pragma comment(lib, "ntdll.lib")

extern "C" {
typedef LONG NTSTATUS, *PNTSTATUS;
#define STATUS_SUCCESS (0x00000000)

// Windows 2000 and newer
NTSYSAPI NTSTATUS NTAPI RtlGetVersion(PRTL_OSVERSIONINFOEXW lpVersionInformation);
}
#endif

CCSprite* BetterInfo::createWithBISpriteFrameName(const char* name){
    return createBISprite(name);
}

CCSprite* BetterInfo::createBISprite(const char* name){
    name = Mod::get()->expandSpriteName(name).data();
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

CCMenuItemSpriteExtra* BetterInfo::createSearchButton(cocos2d::CCLayer* parent, const char* text, const char* icon, cocos2d::SEL_MenuHandler handler, float scale, float iconScale){
    auto searchTexture = SearchButton::create(
        "GJ_longBtn04_001.png",
        text,
        scale,
        icon
    );
    if(searchTexture->m_icon) {
        auto ogSize = searchTexture->m_icon->getScaledContentSize();
        searchTexture->m_icon->setScale(iconScale);

        searchTexture->m_icon->setPositionX(searchTexture->m_icon->getPositionX() + (ogSize.width - searchTexture->m_icon->getScaledContentSize().width) / 4);
        searchTexture->m_label->setPositionX(searchTexture->m_label->getPositionX() + (ogSize.width - searchTexture->m_icon->getScaledContentSize().width) / 4);
    }
    
    //auto searchTexture = CCSprite::createWithSpriteFrameName(texture);
    auto searchBtn = CCMenuItemSpriteExtra::create(
        searchTexture,
        parent,
        handler
    );
    return searchBtn;
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
    || searchObject->m_searchType == SearchType::FavouriteLevels
    || searchObject->m_searchType == SearchType::SmartTemplates
    || searchObject->m_searchType == SearchType::MyLists
    || searchObject->m_searchType == SearchType::FavouriteLists;
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
    for(unsigned int i = 0; i <= 5; i++){
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
        "user_search_uncompletedcoins", "user_search_legendary", "user_search_mythic",
        "user_search_gameversion", "user_search_verifiedcoins", "user_search_unverifiedcoins"
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
    auto songInfo = static_cast<SongInfoObject*>(MusicDownloadManager::sharedState()->m_songObjects->objectForKey(std::to_string(audioID)));
    if(!songInfo) return "";
    return songInfo->m_songUrl;
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
    if(!validateRangeItem(searchObj.coins, level->m_coins)) return false;
    if(searchObj.noCoins && level->m_coins != 0) return false;
    if(searchObj.unverifiedCoins && level->m_coinsVerified) return false;
    if(searchObj.verifiedCoins && !level->m_coinsVerified) return false;
    if(searchObj.epic && level->m_isEpic != 1) return false;
    if(searchObj.legendary && level->m_isEpic != 2) return false;
    if(searchObj.mythic && level->m_isEpic != 3) return false;
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
    if(searchObj.favorite && (!levelFromSaved || !levelFromSaved->m_levelFavorited)) return false;
    if(searchObj.folder > 0 && (!levelFromSaved || levelFromSaved->m_levelFolder != searchObj.folder)) return false;

    return true;
}

std::vector<GJGameLevel*> BetterInfo::completedLevelsInStarRange(int min, int max, bool platformer, CCDictionary* dict) {
    std::vector<GJGameLevel*> levels;

    for(auto [key, level] : CCDictionaryExt<gd::string, GJGameLevel*>(dict)) {
        if(level->m_normalPercent < 100) continue;
        if(level->m_stars < min || level->m_stars > max) continue;
        if((platformer && !level->isPlatformer()) || (!platformer && level->isPlatformer())) continue;

        levels.push_back(level);
    }

    return levels;
}

void BetterInfo::reloadUsernames(LevelBrowserLayer* levelBrowserLayer) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto listLayer = levelBrowserLayer->getChildByType<GJListLayer>(0);
    if(!listLayer) return;
    auto listView = listLayer->getChildByType<CustomListView>(0);
    if(!listView) return;
    auto tableView = listView->getChildByType<TableView>(0);
    if(!tableView) return;
    auto contentLayer = tableView->getChildByType<CCContentLayer>(0);
    if(!contentLayer) return;
    auto children = CCArrayExt<CCNode*>(contentLayer->getChildren());

    for(auto& child : children) {
        //TODO: rewrite the positioning code here

        auto levelCell = typeinfo_cast<LevelCell*>(child);
        if(!levelCell) continue;
        auto menu = levelCell->m_mainLayer->getChildByID("main-menu");
        if(!menu) continue;
        auto playerName = menu->getChildByID("creator-name");
        if(!playerName) continue;
        auto textNode = static_cast<CCLabelBMFont*>(playerName->getChildren()->objectAtIndex(0));
        if(!textNode) continue;

        float oldXSize = textNode->getScaledContentSize().width;

        auto userName = GameLevelManager::sharedState()->userNameForUserID(levelCell->m_level->m_userID);
        if(userName.empty() || std::string(userName) == "-" || std::string(userName) == "Unknown") continue;

        auto oldString = std::string(textNode->getString());
        auto newString = fmt::format("{}{}", levelCell->m_compactView ? "" : "By ", std::string(userName));

        textNode->setString(newString.c_str());

        float difference = textNode->getScaledContentSize().width - oldXSize;

        playerName->setContentSize(textNode->getContentSize() * textNode->getScale());
        playerName->setPositionX(playerName->getPositionX() + (difference / 2));

        textNode->setPositionX(playerName->getContentSize().width / 2);

        if(auto copyIcon = menu->getChildByID("copy-indicator")) {
            copyIcon->setPositionX(copyIcon->getPositionX() + difference);
        }
        if(auto highObjectIcon = menu->getChildByID("high-object-indicator")) {
            highObjectIcon->setPositionX(highObjectIcon->getPositionX() + difference);
        }

        levelCell->m_level->m_creatorName = userName;
    }
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
    default:
        return 201;
        break;
    case 1:
        return 200;
        break;
    case 2:
        return 202;
        break;
    case 3:
        return 203;
        break;
    case 4:
        return 1334;
        break;
    }
}

uint64_t BetterInfo::timeInMs() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

float BetterInfo::timeForLevelString(const std::string& levelString) {
        std::string epicString;
    try {
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
            bool checked = false;

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
                    if(keyID == "1") objID = BetterInfo::stoi(currentKey);
                    else if(keyID == "2") xPos = BetterInfo::stof(currentKey);
                    else if(keyID == "13") checked = BetterInfo::stoi(currentKey);
                    else if(keyID == "kA4") prevPortalId = speedToPortalId(BetterInfo::stoi(currentKey));
                }
                i++;

                if(xPos != 0 && objID != 0 && checked == true) break;
            }

            if(maxPos < xPos) maxPos = xPos;
            if(!checked || !objectIDIsSpeedPortal(objID)) continue;

            timeFull += (xPos - prevPortalX) / travelForPortalId(prevPortalId);
            prevPortalId = objID;
            prevPortalX = xPos;
        }

        timeFull += (maxPos - prevPortalX) / travelForPortalId(prevPortalId);
        auto b = timeInMs() - a;
        log::info("Time for levelString: {}ms, decompress: {}ms, parse: {}ms", b, c - a, timeInMs() - c);
        return timeFull;
    } catch(std::exception e) {
        log::error("An exception has occured while calculating time for levelString: {}", e.what());
        return 0;
    }
}

bool BetterInfo::controllerConnected() {
    return PlatformToolbox::isControllerConnected();
}

// https://www.winehq.org/pipermail/wine-devel/2008-September/069387.html
std::string BetterInfo::getWineVersion() {
    #ifdef GEODE_IS_WINDOWS
        static const char * (CDECL *pwine_get_version)(void);
        HMODULE hntdll = GetModuleHandle("ntdll.dll");
        if(!hntdll)
        {
            return "";
        }
        pwine_get_version = (const char *(__cdecl *)(void))GetProcAddress(hntdll, "wine_get_version");
        if(pwine_get_version)
        {
            return fmt::format("Wine%20{}", pwine_get_version());
        }
        
        return "";

    #else
    return "";
    #endif
}

std::string BetterInfo::getOSVersion() {
    #ifdef GEODE_IS_WINDOWS
        // https://stackoverflow.com/questions/25986331/how-to-determine-windows-version-in-future-proof-way
        RTL_OSVERSIONINFOEXW osVers;
        osVers.dwOSVersionInfoSize = sizeof(osVers);

        NTSTATUS status = RtlGetVersion(&osVers);

        return fmt::format("Windows%20{}.{}.{}", osVers.dwMajorVersion, osVers.dwMinorVersion, osVers.dwBuildNumber);
    #else
        return GEODE_PLATFORM_NAME;
    #endif
}

void BetterInfo::loadImportantNotices(Ref<CCLayer> layer) {
    static bool hasBeenCalled = false;
    if(hasBeenCalled) return;
    hasBeenCalled = true;

    auto url = fmt::format("https://geometrydash.eu/mods/betterinfo/_api/importantNotices/?platform={}&version={}&loader={}&wine={}&os={}&notAlpha7=1", GEODE_PLATFORM_NAME, Mod::get()->getVersion().toVString(true), Loader::get()->getVersion().toVString(true), getWineVersion(), getOSVersion());
    static std::optional<web::WebTask> s_requestTask = std::nullopt;

    s_requestTask = web::WebRequest().get(url).map(
        [layer](web::WebResponse* response) {
            if(!response->ok() || response->json().isErr()) {
                log::warn("Fetching important notices failed: {} - {}", response->code(), response->string().unwrapOr("No response"));
                return *response;
            }

            auto info = response->json().unwrap();

            auto notice = info.try_get("notice");
            if(notice == std::nullopt) return *response;
            
            if(info["notice"].is_string()) {
                auto alert = FLAlertLayer::create("BetterInfo", info["notice"].as_string(), "OK");
                alert->m_scene = layer;
                alert->show();
            }

            return *response;
        });
}

FLAlertLayer* BetterInfo::createUpdateDialog() {
    //TODO: enable before real 420 release
    //return nullptr;

    auto versionResult = VersionInfo::parse(
        Mod::get()->getSavedValue<std::string>(
            "last_dialog_version",
            Mod::get()->getSavedValue<std::string>(
                "last_launch_version",
                Mod::get()->getVersion().toVString()
            )
        )
    );
    //if(versionResult.isOk() && versionResult->getMinor() == Mod::get()->getVersion().getMinor()) return nullptr;
    if(versionResult.isOk() && ComparableVersionInfo::parse(">=4.2.0")->compare(versionResult.value())) return nullptr;

    return createQuickPopup(
        "BetterInfo",
        "<cg>BetterInfo has updated!</c>\n"
        "\n"
        "<cy>Changelog:</c> <cg>v4.2.0</c> (2024-07-07)\n"
        "- <cg>Added</c> <cl>list folders</c>\n"
        "- <cg>Added</c> <cl>notification</c> when you can claim a <co>rated list reward</c>\n"
        "- <cg>Added</c> <cl>followed user list</c> to <co>friends menu</c>\n"
        "- <cg>Added</c> <cl>Coin Count</c> and <cl>Folder</c> filters to <cj>Filtered Search</c>\n"
        "- <cg>Added</c> more <cl>Level Progress</c> information to <co>Editor levels</c>\n"
        "- <cr>More!</c> Click \"<cy>More Info</c>\" for the <co>whole list</c>!\n"
        "\n",
        "More Info",
        "Ok",
        420,
        [](FLAlertLayer* me, bool btn2) {
            Mod::get()->setSavedValue<std::string>("last_dialog_version", Mod::get()->getVersion().toVString());

            if(!btn2) {
                openChangelogPopup(Mod::get());
            }
        },
        false
    );
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

CCMenuItemSpriteExtra* BetterInfo::replaceWithButton(CCNode* node, CCNode* self, cocos2d::SEL_MenuHandler handler) {

    auto parent = node->getParent();

    auto idx = parent->getChildren()->indexOfObject(node);
    node->removeFromParent();

    for(auto i = idx; i < parent->getChildrenCount(); i++) {
        auto child = static_cast<CCNode*>(parent->getChildren()->objectAtIndex(i));
        child->setZOrder(child->getZOrder() + 1);
    }

    auto button = CCMenuItemSpriteExtra::create(
        node,
        self,
        handler
    );
    button->setLayoutOptions(copyLayoutOptions(node));
    button->setZOrder(node->getZOrder());
    button->setID(node->getID());

    parent->addChild(button);
    parent->updateLayout();

    node->setScale(1.f);

    return button;
}

//https://github.com/geode-sdk/DevTools/blob/0b12d41a6906dd352b50fd363f6dea28121c3a57/src/pages/Tree.cpp#L10
std::string getNodeNameImpl(CCObject* node) {
#ifdef GEODE_IS_WINDOWS
    return typeid(*node).name() + 6;
#else 
    {
        std::string ret;

        int status = 0;
        auto demangle = abi::__cxa_demangle(typeid(*node).name(), 0, 0, &status);
        if (status == 0) {
            ret = demangle;
        }
        free(demangle);

        return ret;
    }
#endif
}

std::string BetterInfo::getNodeName(CCObject* node, bool spr) {
    auto nodeName = getNodeNameImpl(node);

    if(spr) nodeName = Mod::get()->expandSpriteName(nodeName);
    return nodeName;
}

UnlockType BetterInfo::iconTypeToUnlockType(IconType type) {
    //the actual func doesnt work for an unknown reason
    int result = 0;

    switch ((int) type)
    {
        case 0:
            result = 1;
            break;
        case 1:
            result = 4;
            break;
        case 2:
            result = 5;
            break;
        case 3:
            result = 6;
            break;
        case 4:
            result = 7;
            break;
        case 5:
            result = 8;
            break;
        case 6:
            result = 9;
            break;
        case 7:
            result = 13;
            break;
        case 8:
            result = 14;
            break;
        case 9:
            result = 11;
            break;
        case 0xA:
            result = 10;
            break;
        case 0xB:
            result = 12;
            break;
        case 0xC:
            result = 15;
            break;
        default:
            result = 0;
            break;
    }
    return (UnlockType) result;
}

bool BetterInfo::isLevelSearchObject(GJSearchObject* object) {
    if(object->m_searchType == SearchType::UsersLevels) return object->m_searchMode == 0;

    return object->isLevelSearchObject();
}

AxisLayoutOptions* BetterInfo::copyLayoutOptions(CCNode* a) {
    return copyLayoutOptions(typeinfo_cast<AxisLayoutOptions*>(a->getLayoutOptions()));
}

AxisLayoutOptions* BetterInfo::copyLayoutOptions(AxisLayoutOptions* a) {
    if(!a) return nullptr;

    return AxisLayoutOptions::create()
        ->setScaleLimits(a->getMinScale(), a->getMaxScale())
        ->setRelativeScale(a->getRelativeScale())
        ->setLength(a->getLength())
        ->setPrevGap(a->getPrevGap())
        ->setNextGap(a->getNextGap())
        ->setBreakLine(a->getBreakLine())
        ->setSameLine(a->getSameLine())
        ->setScalePriority(a->getScalePriority());
}

int BetterInfo::stoi(std::string_view str) {
    int result = 0;
    std::from_chars(str.data(), str.data() + str.size(), result);
    return result;
}

float BetterInfo::stof(std::string_view str) {
    return utils::numFromString<float>(str).unwrapOr(0);
}

long long BetterInfo::strtol(std::string_view str) {
    long long result = 0;
    std::from_chars(str.data(), str.data() + str.size(), result);
    return result;
}

static std::vector<Ref<Notification>> s_notifications;
void BetterInfo::showUnimportantNotification(const std::string& content, NotificationIcon icon, float time) {
    if(GJBaseGameLayer::get()) return;

    auto notif = Notification::create(content, icon, time);
    s_notifications.push_back(notif);
    notif->show();

    std::thread([notif, time] {
        //assume up to 4 notifications are scheduled outside of this - if this fails nothing much really happens, they just wont be removed immediately
        //assume notifications are not longer than 5s
        std::this_thread::sleep_for(std::chrono::seconds((int) (5 * (s_notifications.size() + 4))));
        Loader::get()->queueInMainThread([notif] {
            s_notifications.erase(std::remove(s_notifications.begin(), s_notifications.end(), notif), s_notifications.end());
        });
    }).detach();
}

void BetterInfo::cancelUnimportantNotifications() {
    for(auto& notification : s_notifications) {
        notification->cancel();
    }
    s_notifications.clear();
}