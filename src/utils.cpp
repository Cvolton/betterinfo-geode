#include "utils.hpp"

#include <random>

#include <Geode/cocos/support/base64.h>
#include <Geode/cocos/support/zip_support/ZipUtils.h>

CCSprite* BetterInfo::createWithBISpriteFrameName(const char* name){
        //TODO: geodify
        auto sprite = CCSprite::createWithSpriteFrameName(name);
        if(sprite) return sprite;

        return createPlaceholder();
}

CCSprite* BetterInfo::createBISprite(const char* name){
        //TODO: geodify
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

int BetterInfo::randomNumber(int start, int end){
    std::random_device os_seed;
    const unsigned int seed = os_seed();

    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> distribute(start, end);

    return distribute(generator);
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

bool BetterInfo::isSavedFiltered() {
        //TODO: implement
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
        return std::format("{}-{:02}-{:02} {:02}:{:02}", time3.tm_year + 1900, time3.tm_mon + 1, time3.tm_mday, time3.tm_hour, time3.tm_min);
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