#include "ProfileSearchOptions.h"
#include "Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDelegate.h"
#include "ProfileSearchOptionsSongSelect.h"
#include "IDRangePopup.h"
#include "GameVersionPopup.h"

#include <fmt/format.h>

ProfileSearchOptions* ProfileSearchOptions::create(LevelBrowserLayer* levelBrowserLayer, const std::string& prefix, BISearchObjectDelegate* searchObjDelegate){
    auto ret = new ProfileSearchOptions();
    if (ret && ret->init(levelBrowserLayer, prefix, searchObjDelegate)) {
        //robert 1 :D
        ret->autorelease();
    } else {
        //robert -1
        delete ret;
        ret = nullptr;
    }
    return ret;
}

void ProfileSearchOptions::onClose(cocos2d::CCObject* sender)
{
    this->retain();
    CvoltonOptionsLayer::onClose(sender);

    if(m_searchObjDelegate != nullptr) m_searchObjDelegate->onSearchObjectFinished(getSearchObject());
    if(m_levelBrowserLayer != nullptr) {
        m_levelBrowserLayer->loadPage(m_levelBrowserLayer->m_searchObject);
    }
    this->release();
}

void ProfileSearchOptions::onPrev(cocos2d::CCObject* sender)
{
    m_page -= 1;
    if(m_page < 0) m_page = 0;
    destroyToggles();
    drawToggles();
}

void ProfileSearchOptions::onSong(cocos2d::CCObject* sender)
{
    ProfileSearchOptionsSongSelect::create(this)->show();
}

void ProfileSearchOptions::onIdRange(cocos2d::CCObject* sender)
{
    IDRangePopup::create(this, getOptionInt("idrange_min"), getOptionInt("idrange_max"), "ID Range")->show();
}

void ProfileSearchOptions::onStarRange(cocos2d::CCObject* sender)
{
    IDRangePopup::create(this, getOptionInt("starrange_min"), getOptionInt("starrange_max"), "Star Range", 1)->show();
}

void ProfileSearchOptions::onPercentage(cocos2d::CCObject* sender)
{
    IDRangePopup::create(this, getOptionInt("percentage_min"), getOptionInt("percentage_max"), "Percentage", 2)->show();
}

void ProfileSearchOptions::onPercentageOrbs(cocos2d::CCObject* sender)
{
    IDRangePopup::create(this, getOptionInt("percentageorbs_min"), getOptionInt("percentageorbs_max"), "% Orbs", 3)->show();
}

void ProfileSearchOptions::onPercentageLeaderboard(cocos2d::CCObject* sender)
{
    IDRangePopup::create(this, getOptionInt("percentageleaderboard_min"), getOptionInt("percentageleaderboard_max"), "% Leaderboard", 4)->show();
}

void ProfileSearchOptions::onGameVersionRange(cocos2d::CCObject* sender)
{
    GameVersionPopup::create(this, getOptionInt("gameversion_min"), getOptionInt("gameversion_max"), 5)->show();
}

void ProfileSearchOptions::onCoins(cocos2d::CCObject* sender)
{
    IDRangePopup::create(this, getOptionInt("coins_min"), getOptionInt("coins_max"), "Coins", 7)->show();
}

void ProfileSearchOptions::onFolder(cocos2d::CCObject* sender)
{
    auto popup = SetFolderPopup::create(getOptionInt("folder_min"), false, "Go to Folder");
    popup->m_scene = this;
    popup->m_delegate = this;
    popup->show();
}

void ProfileSearchOptions::onNext(cocos2d::CCObject* sender)
{
    m_page += 1;
    destroyToggles();
    drawToggles();
}

void ProfileSearchOptions::onSecondaryInfo(cocos2d::CCObject* sender){
    FLAlertLayer::create(
        nullptr, 
        "Level Filters", 
        "<cl>Demon Difficulty</c> filters are only active if you have <cg>enabled</c> the main <cy>Demon</c> filter on the 1st page.",
        "OK", 
        nullptr,
        350
    )->show();
}

bool ProfileSearchOptions::init(LevelBrowserLayer* levelBrowserLayer, const std::string& prefix, BISearchObjectDelegate* searchObjDelegate){
    if(!CvoltonAlertLayerStub::init({440.0f, 290.0f}, 1.f, 0x96)) return false;

    m_columns = 3;
    m_top = 96;
    m_gap = 39;

    this->m_scene = levelBrowserLayer;
    this->m_levelBrowserLayer = levelBrowserLayer;
    this->m_prefix = prefix;
    this->m_searchObjDelegate = searchObjDelegate;

    Mod::get()->getSavedValue<int>(fmt::format("{}_{}", m_prefix, "gameversion_max"), INT_MAX);

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto title = createTextLabel("Level Filters", {(winSize.width / 2), (winSize.height / 2) + 127}, .7f, m_mainLayer, "goldFont.fnt");
    title->setID("title-label"_spr);

    m_lengthBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f });
    m_lengthBg->setContentSize({700,55});
    m_lengthBg->setColor({123,60,31});
    m_buttonMenu->addChild(m_lengthBg, -1);
    m_lengthBg->setPosition({0,-120});
    m_lengthBg->setScale(0.6f);
    m_lengthBg->setID("length-background"_spr);

    m_diffBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f });
    m_diffBg->setContentSize({700,92});
    m_diffBg->setColor({123,60,31});
    m_buttonMenu->addChild(m_diffBg, -1);
    m_diffBg->setPosition({0,-70});
    m_diffBg->setScale(0.6f);
    m_diffBg->setID("difficulty-background"_spr);

    m_demonDiffBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f });
    m_demonDiffBg->setContentSize({700,93});
    m_demonDiffBg->setColor({123,60,31});
    m_buttonMenu->addChild(m_demonDiffBg, -1);
    m_demonDiffBg->setPosition({0,-105.5});
    m_demonDiffBg->setScale(0.6f);
    m_demonDiffBg->setID("demon-difficulty-background"_spr);

    auto sprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    sprite->setFlipX(true);

    m_prevBtn = createButton("GJ_arrow_01_001.png", {- (winSize.width / 2) + 30, 0}, menu_selector(ProfileSearchOptions::onPrev));
    m_prevBtn->setID("prev-button"_spr);
    m_nextBtn = CCMenuItemSpriteExtra::create(
        sprite,
        this,
        menu_selector(ProfileSearchOptions::onNext)
    );
    m_nextBtn->setPosition({(winSize.width / 2) - 30, 0});
    m_nextBtn->setID("next-button"_spr);
    m_buttonMenu->addChild(m_nextBtn);

    m_lenMenu = CCMenu::create();
    m_lenMenu->setZOrder(10);
    m_lenMenu->setContentSize({330, 30});
    m_lenMenu->setPosition({(winSize.width / 2) , (winSize.height / 2) - 120});
    m_lenMenu->setLayout(
        RowLayout::create()
            ->setGap(20.f)
    );
    m_lenMenu->setID("length-menu"_spr);
    m_mainLayer->addChild(m_lenMenu);

    drawToggles();

    return true;
}

void ProfileSearchOptions::reloadBrowser(){
    Mod::get()->setSavedValue<bool>("user_search_dirty", true);
    if(m_levelBrowserLayer != nullptr) {
        auto searchObj = m_levelBrowserLayer->m_searchObject;
        int pageBegin = (searchObj->m_page * BetterInfo::levelsPerPage(searchObj)) + 1;
        int total = GameLevelManager::get()->getSavedLevels(searchObj->m_searchType == SearchType::FavouriteLevels, searchObj->m_folder)->count();
        int pageEnd = std::min( (searchObj->m_page + 1) * BetterInfo::levelsPerPage(searchObj) , total);

        //m_levelBrowserLayer->loadPage(m_levelBrowserLayer->m_searchObject);
        if(m_levelBrowserLayer->m_countText) m_levelBrowserLayer->m_countText->setString(
            fmt::format("{}{} to {} of {}", BetterInfo::isSavedFiltered() ? "(Filtered) " : "", pageBegin, pageEnd, total).c_str()
        );
    }
}

void ProfileSearchOptions::drawToggles(){
    switch(m_page % 3) {
        default:
            drawTogglesPrimary();
            break;
        case 1:
            drawTogglesSecondary();
            break;
        case 2:
            drawTogglesTerciary();
            break;
    }

    reloadBrowser();
}

void ProfileSearchOptions::drawTogglesPrimary(){
    m_lengthBg->setVisible(true);
    m_prevBtn->setVisible(false);
    m_nextBtn->setVisible(true);
    m_demonDiffBg->setVisible(false);
    m_diffBg->setVisible(true);

    auto winSize = CCDirector::sharedDirector()->getWinSize();


    auto timeIcon = CCSprite::createWithSpriteFrameName("GJ_timeIcon_001.png");
    timeIcon->setID("time-icon"_spr);
    timeIcon->setPosition({-193, -120});
    m_buttonMenu->addChild(timeIcon);
    m_toggles.push_back(timeIcon);

    createButtonToggle("len_00", CCLabelBMFont::create("Tiny", "bigFont.fnt"), -142, -119, 0.6f, m_lenMenu);
    createButtonToggle("len_01", CCLabelBMFont::create("Short", "bigFont.fnt"), -69, -119, 0.6f, m_lenMenu);
    createButtonToggle("len_02", CCLabelBMFont::create("Medium", "bigFont.fnt"), 16, -119, 0.6f, m_lenMenu);
    createButtonToggle("len_03", CCLabelBMFont::create("Long", "bigFont.fnt"), 93.5f, -119, 0.6f, m_lenMenu);
    createButtonToggle("len_04", CCLabelBMFont::create("XL", "bigFont.fnt"), 150, -119, 0.6f, m_lenMenu);
    createButtonToggle("len_05", CCLabelBMFont::create("Plat.", "bigFont.fnt"), 150, -119, 0.6f, m_lenMenu);
    createButtonToggle("star", CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png"), 193, -120);

    m_lenMenu->updateLayout();

    for(unsigned int i = 0; i <= 6; i++){
        createButtonToggle(
            CCString::createWithFormat("diff_%02u", i)->getCString(),
            CCSprite::createWithSpriteFrameName(CCString::createWithFormat("difficulty_%02u_btn_001.png", i)->getCString()),
            -187 + (i * 53.5f),
            -70,
            .9f
        );
    }
    createButtonToggle("diff_auto", CCSprite::createWithSpriteFrameName("difficulty_auto_btn_001.png"), 187, -70, .9f);

    //40 -60, 170 -60, 300 -60, 40 -110
    createToggle("featured", "Featured");
    createToggle("epic", "Epic");
    createToggle("legendary", "Legendary");
    createToggle("mythic", "Mythic");
    createToggle("original", "Original");
    createToggle("song", "Song", menu_selector(ProfileSearchOptions::onSong));
    createToggle("nofeatured", "Not Featured");
    createToggle("noepic", "Not Epic");
    createToggle("nostar", "No Star");
    createToggle("coins", "Has Coins", menu_selector(ProfileSearchOptions::onCoins));
    createToggle("nocoins", "No Coins");
    createToggle("verifiedcoins", "Verified Coins");
}

void ProfileSearchOptions::drawTogglesSecondary(){
    m_lengthBg->setVisible(false);
    m_prevBtn->setVisible(true);
    m_nextBtn->setVisible(true);
    m_demonDiffBg->setVisible(true);
    m_diffBg->setVisible(false);

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    for(unsigned int i = 0, diffSprite = 7; i <= 4; i++, diffSprite++){
        if(i == 2) diffSprite = 6;
        if(i == 3) diffSprite = 9;

        createButtonToggle(
            CCString::createWithFormat("demon_%02u", i)->getCString(),
            CCSprite::createWithSpriteFrameName(CCString::createWithFormat("difficulty_%02u_btn2_001.png", diffSprite)->getCString()),
            -133.5f + (i * 66.875f),
            -102,
            .9f
        );
    }

    auto infoBtn = createButton("GJ_infoIcon_001.png", {203, 128}, menu_selector(ProfileSearchOptions::onSecondaryInfo));
    infoBtn->setID("info-button"_spr);
    m_toggles.push_back(infoBtn);

    createToggle("copied", "Copied");
    createToggle("downloaded", "Downloaded");
    createToggle("starrange", "Star Range", menu_selector(ProfileSearchOptions::onStarRange));
    createToggle("idrange", "ID Range", menu_selector(ProfileSearchOptions::onIdRange));
    if(!m_prefix.empty()) createToggle("copy", "Copyable");
    if(!m_prefix.empty()) createToggle("copy_free", "Free Copy");
    if(!m_prefix.empty()) createToggle("ldm", "LDM");
    createToggle("twoplayer", "2-Player");
    createToggle("gameversion", "Game Ver.", menu_selector(ProfileSearchOptions::onGameVersionRange));
    createToggle("unverifiedcoins", "Unverified Coins");
    
}

void ProfileSearchOptions::drawTogglesTerciary(){
    m_lengthBg->setVisible(false);
    m_prevBtn->setVisible(true);
    m_nextBtn->setVisible(false);
    m_demonDiffBg->setVisible(false);
    m_diffBg->setVisible(false);

    createToggle("completed", "Completed"); 
    createToggle("completedorbs", "C. Orbs");
    createToggle("completedleaderboard", "C. Leaderboard");

    createToggle("uncompleted", "Uncompleted"); 
    createToggle("uncompletedorbs", "Uc. Orbs");
    createToggle("uncompletedleaderboard", "Uc. Leaderboard");

    createToggle("percentage", "Percentage", menu_selector(ProfileSearchOptions::onPercentage));
    createToggle("percentageorbs", "% Orbs", menu_selector(ProfileSearchOptions::onPercentageOrbs));
    createToggle("percentageleaderboard", "% Leaderboard", menu_selector(ProfileSearchOptions::onPercentageLeaderboard));

    createToggle("completedcoins", "C. Coins"); 
    createToggle("uncompletedcoins", "Uc. Coins");
    if(m_prefix.empty()) createToggle("favorite", "Favorited");
    if(m_prefix.empty()) createToggle("folder", "Folder", menu_selector(ProfileSearchOptions::onFolder));
}

void ProfileSearchOptions::onSongDialogClosed(bool custom, int songID){
    setOption("song_custom", custom);
    setOptionInt("song_id", songID);

    reloadBrowser();
}

void ProfileSearchOptions::onIDRangeFinished(int min, int max, int additional) {
    const char* option = "idrange";
    switch(additional) {
        case 1:
            option = "starrange";
            break;
        case 2:
            option = "percentage";
            break;
        case 3:
            option = "percentageorbs";
            break;
        case 4:
            option = "percentageleaderboard";
            break;
        case 5:
            option = "gameversion";
            break;
        case 6:
            option = "folder";
            break;
        case 7:
            option = "coins";
            break;
    }

    setOptionInt(fmt::format("{}_min", option), min);
    setOptionInt(fmt::format("{}_max", option), max);
    
    reloadBrowser();
}

void ProfileSearchOptions::setIDPopupClosed(SetIDPopup* popup, int value) {
    onIDRangeFinished(value, INT_MIN, 6);
}

bool ProfileSearchOptions::getOption(const std::string& option) const {
    if(!m_prefix.empty()) return Mod::get()->getSavedValue<bool>(fmt::format("{}_{}", m_prefix, option));

    if(m_options.contains(option)) return m_options.at(option);
    return false;
}

int ProfileSearchOptions::getOptionInt(const std::string& option) const {
    if(!m_prefix.empty()) return Mod::get()->getSavedValue<int>(fmt::format("{}_{}", m_prefix, option));

    if(m_optionInts.contains(option)) return m_optionInts.at(option);
    return 0;
}

bool ProfileSearchOptions::toggleOption(const std::string& option) {
    setOption(option, !getOption(option));
    return getOption(option);
}

void ProfileSearchOptions::setOption(const std::string& option, bool value) {
    if(!m_prefix.empty()) Mod::get()->setSavedValue<bool>(fmt::format("{}_{}", m_prefix, option), value);

    m_options[option] = value;
}

void ProfileSearchOptions::setOptionInt(const std::string& option, int value) {
    if(!m_prefix.empty()) Mod::get()->setSavedValue<int>(fmt::format("{}_{}", m_prefix, option), value);

    m_optionInts[option] = value;
}

BISearchObject ProfileSearchOptions::getSearchObject() {
    BISearchObject searchObj;

    if(getOption("diff_auto")) searchObj.difficulty.insert(-1);
    for(int i = 0; i <= 6; i++) {
        auto diff = CCString::createWithFormat("diff_%02i", i)->getCString();
        if(getOption(diff)) searchObj.difficulty.insert(i);
    }

    for(int i = 0; i <= 5; i++) {
        auto len = CCString::createWithFormat("len_%02i", i)->getCString();
        if(getOption(len)) searchObj.length.insert(i);
    }

    for(int i = 0; i <= 4; i++) {
        auto len = CCString::createWithFormat("demon_%02i", i)->getCString();
        if(getOption(len)) searchObj.demonDifficulty.insert(i);
    }

    searchObj.star = getOption("star");
    searchObj.noStar = getOption("nostar");
    searchObj.uncompleted = getOption("uncompleted");
    searchObj.uncompletedOrbs = getOption("uncompletedorbs");
    searchObj.uncompletedLeaderboard = getOption("uncompletedleaderboard");
    searchObj.uncompletedCoins = getOption("uncompletedcoins");
    searchObj.completed = getOption("completed");
    searchObj.completedOrbs = getOption("completedorbs");
    searchObj.completedLeaderboard = getOption("completedleaderboard");
    searchObj.completedCoins = getOption("completedcoins");

    setToRangeItem(searchObj.percentage, "percentage");
    setToRangeItem(searchObj.percentageOrbs, "percentageorbs");
    setToRangeItem(searchObj.percentageLeaderboard, "percentageleaderboard");
    setToRangeItem(searchObj.coins, "coins");

    searchObj.featured = getOption("featured");
    searchObj.original = getOption("original");
    searchObj.twoPlayer = getOption("twoplayer");
    searchObj.noCoins = getOption("nocoins");
    searchObj.verifiedCoins = getOption("verifiedcoins");
    searchObj.unverifiedCoins = getOption("unverifiedcoins");
    searchObj.epic = getOption("epic");
    searchObj.legendary = getOption("legendary");
    searchObj.mythic = getOption("mythic");
    searchObj.folder = getOption("folder") ? getOptionInt("folder_min") : 0;
    searchObj.song = getOption("song");
    searchObj.songCustom = getOption("song_custom");
    searchObj.songID = getOptionInt("song_id");
    searchObj.copied = getOption("copied");
    searchObj.downloaded = getOption("downloaded");
    searchObj.ldm = getOption("ldm");
    setToRangeItem(searchObj.idRange, "idrange");
    searchObj.copyable = getOption("copy");
    searchObj.freeCopy = getOption("copy_free");
    searchObj.unfeatured = getOption("nofeatured");
    searchObj.unepic = getOption("noepic");
    searchObj.favorite = getOption("favorite");

    setToRangeItem(searchObj.starRange, "starrange");
    setToRangeItem(searchObj.gameVersion, "gameversion");

    return searchObj;
}

void ProfileSearchOptions::setSearchObject(const BISearchObject& searchObj) {
    //this is slowly getting into ObjectToolbox::init territory...

    for(int i = -1; i <= 6; i++) {
        setOption(
            i == -1 ? "diff_auto" : CCString::createWithFormat("diff_%02i", i)->getCString(),
            searchObj.difficulty.find(i) != searchObj.difficulty.end()
        );
    }

    for(int i = 0; i <= 5; i++) {
        setOption(
            CCString::createWithFormat("len_%02i", i)->getCString(),
            searchObj.length.find(i) != searchObj.length.end()
        );
    }

    for(int i = 0; i <= 4; i++) {
        setOption(
            CCString::createWithFormat("demon_%02i", i)->getCString(),
            searchObj.demonDifficulty.find(i) != searchObj.demonDifficulty.end()
        );
    }

    setOption("star", searchObj.star);
    setOption("nostar", searchObj.noStar);
    setOption("uncompleted", searchObj.uncompleted);
    setOption("uncompletedorbs", searchObj.uncompletedOrbs);
    setOption("uncompletedleaderboard", searchObj.uncompletedLeaderboard);
    setOption("uncompletedcoins", searchObj.uncompletedCoins);
    setOption("completed", searchObj.completed);
    setOption("completedorbs", searchObj.completedOrbs);
    setOption("completedleaderboard", searchObj.completedLeaderboard);
    setOption("completedcoins", searchObj.completedCoins);
    setFromRangeItem("percentage", searchObj.percentage);
    setFromRangeItem("percentageorbs", searchObj.percentageOrbs);
    setFromRangeItem("percentageleaderboard", searchObj.percentageLeaderboard);
    setFromRangeItem("coins", searchObj.coins);
    setOption("featured", searchObj.featured);
    setOption("original", searchObj.original);
    setOption("twoplayer", searchObj.twoPlayer);
    setOption("nocoins", searchObj.noCoins);
    setOption("verifiedcoins", searchObj.verifiedCoins);
    setOption("unverifiedcoins", searchObj.unverifiedCoins);
    setOption("epic", searchObj.epic);
    setOption("legendary", searchObj.legendary);
    setOption("mythic", searchObj.mythic);
    setOptionInt("folder_min", searchObj.folder);
    if(searchObj.folder > 0) setOption("folder", true);
    //searchObj.folder = 0;
    setOption("song", searchObj.song);
    setOption("song_custom", searchObj.songCustom);
    setOptionInt("song_id", searchObj.songID);
    setOption("copied", searchObj.copied);
    setOption("downloaded", searchObj.downloaded);
    setOption("ldm", searchObj.ldm);
    setFromRangeItem("idrange", searchObj.idRange);
    setOption("copy", searchObj.copyable);
    setOption("copy_free", searchObj.freeCopy);
    setOption("nofeatured", searchObj.unfeatured);
    setOption("noepic", searchObj.unepic);
    setOption("favorite", searchObj.favorite);
    setFromRangeItem("starrange", searchObj.starRange);
    setFromRangeItem("gameversion", searchObj.gameVersion);

    destroyToggles();
    drawToggles();
}

void ProfileSearchOptions::setFromRangeItem(const std::string& option, const BISearchObject::RangeItem& item) {
    setOption(option, item.enabled);
    setOptionInt(fmt::format("{}_min", option), item.min);
    setOptionInt(fmt::format("{}_max", option), item.max);
}

void ProfileSearchOptions::setToRangeItem(BISearchObject::RangeItem& item, const std::string& option) const {
    item = {getOption(option), getOptionInt(fmt::format("{}_min", option)), getOptionInt(fmt::format("{}_max", option))};
}

void ProfileSearchOptions::keyDown(enumKeyCodes key, double timestamp) {
    switch(key) {
        case KEY_Left:
        case CONTROLLER_Left:
            if(m_prevBtn->isVisible()) onPrev(nullptr);
            break;
        case KEY_Right:
        case CONTROLLER_Right:
            if(m_nextBtn->isVisible()) onNext(nullptr);
            break;
        default:
            CvoltonOptionsLayer::keyDown(key, timestamp);
            break;
    }
}