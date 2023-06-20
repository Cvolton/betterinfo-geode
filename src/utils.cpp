#include "utils.hpp"

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