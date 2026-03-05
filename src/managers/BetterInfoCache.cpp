#include "BetterInfoCache.h"
#include "BetterInfoOnline.h"
#include <arc/time/Sleep.hpp>
#include <asp/iter.hpp>

/**
 * MATjson shenanigans
 */
template <>
struct matjson::Serialize<BetterInfoCache::CachedLevel> {
    static geode::Result<BetterInfoCache::CachedLevel> fromJson(const matjson::Value& value) {
        GEODE_UNWRAP_INTO(std::string name, value["name"].asString());
        GEODE_UNWRAP_INTO(int coins, value["coins"].asInt());
        GEODE_UNWRAP_INTO(int demonDifficulty, value["demonDifficulty"].asInt());
        return Ok(BetterInfoCache::CachedLevel{name, coins, demonDifficulty});
    }
    static matjson::Value toJson(const BetterInfoCache::CachedLevel& level) {
        return matjson::makeObject({
            {"name", level.m_name},
            {"coins", level.m_coins},
            {"demonDifficulty", level.m_demonDifficulty}
        });
    }
};

template <>
struct matjson::Serialize<BetterInfoCache::CachedLevelDate> {
    static geode::Result<BetterInfoCache::CachedLevelDate> fromJson(const matjson::Value& value) {
        GEODE_UNWRAP_INTO(std::string uploadDate, value["uploadDate"].asString());
        GEODE_UNWRAP_INTO(std::string updateDate, value["updateDate"].asString());
        return Ok(BetterInfoCache::CachedLevelDate{uploadDate, updateDate});
    }
    static matjson::Value toJson(const BetterInfoCache::CachedLevelDate& levelDate) {
        return matjson::makeObject({
            {"uploadDate", levelDate.m_uploadDate},
            {"updateDate", levelDate.m_updateDate}
        });
    }
};

template <>
struct matjson::Serialize<Ref<GJUserScore>> {
    static geode::Result<Ref<GJUserScore>> fromJson(const matjson::Value& value) {
        auto score = GJUserScore::create();
        GEODE_UNWRAP_INTO(score->m_userName, value["name"].asString());
        GEODE_UNWRAP_INTO(score->m_userID, value["userID"].asInt());
        GEODE_UNWRAP_INTO(score->m_accountID, value["accountID"].asInt());
        GEODE_UNWRAP_INTO(score->m_stars, value["stars"].asInt());
        GEODE_UNWRAP_INTO(score->m_moons, value["moons"].asInt());
        GEODE_UNWRAP_INTO(score->m_diamonds, value["diamonds"].asInt());
        GEODE_UNWRAP_INTO(score->m_demons, value["demons"].asInt());
        GEODE_UNWRAP_INTO(score->m_playerRank, value["playerRank"].asInt());
        GEODE_UNWRAP_INTO(score->m_creatorPoints, value["creatorPoints"].asInt());
        GEODE_UNWRAP_INTO(score->m_secretCoins, value["secretCoins"].asInt());
        GEODE_UNWRAP_INTO(score->m_userCoins, value["userCoins"].asInt());
        GEODE_UNWRAP_INTO(score->m_iconID, value["iconID"].asInt());
        GEODE_UNWRAP_INTO(score->m_color1, value["color1"].asInt());
        GEODE_UNWRAP_INTO(score->m_color2, value["color2"].asInt());
        GEODE_UNWRAP_INTO(score->m_color3, value["color3"].asInt());
        GEODE_UNWRAP_INTO(score->m_special, value["special"].asInt());
        GEODE_UNWRAP_INTO(auto iconType, value["iconType"].asInt());
        score->m_iconType = (IconType)iconType;

        return Ok(Ref(score));
    }
    static matjson::Value toJson(const Ref<GJUserScore>& score) {
        return matjson::makeObject({
            {"name", std::string(score->m_userName)},
            {"userID", score->m_userID},
            {"accountID", score->m_accountID},
            {"stars", score->m_stars},
            {"moons", score->m_moons},
            {"diamonds", score->m_diamonds},
            {"demons", score->m_demons},
            {"playerRank", score->m_playerRank},
            {"creatorPoints", score->m_creatorPoints},
            {"secretCoins", score->m_secretCoins},
            {"userCoins", score->m_userCoins},
            {"iconID", score->m_iconID},
            {"color1", score->m_color1},
            {"color2", score->m_color2},
            {"color3", score->m_color3},
            {"special", score->m_special},
            {"iconType", (int)score->m_iconType}
        });
    }
};

// https://github.com/geode-sdk/json/blob/d54b2aba2361f08b08d96d1f302b19cf2153c305/include/matjson/std.hpp#L169
template <class T, class Hash, class KeyEqual, class Alloc>
struct matjson::Serialize<std::unordered_map<int, T, Hash, KeyEqual, Alloc>> {
    using Map = std::unordered_map<int, T, Hash, KeyEqual, Alloc>;

    static geode::Result<Map> fromJson(Value const& value)
        requires requires(Value const& value) { value.template as<std::decay_t<T>>(); }
    {
        if (!value.isObject()) return geode::Err("not an object");
        Map res;
        for (auto const& [k, v] : value) {
            GEODE_UNWRAP_INTO(auto vv, v.template as<std::decay_t<T>>());
            res.insert({BetterInfo::stoi(k), vv});
        }
        return geode::Ok(res);
    }

    static Value toJson(Map const& value)
        requires requires(T const& value) { Value(value); }
    {
        Value res;
        for (auto const& [k, v] : value) {
            res.set(fmt::format("{}", k), Value(v));
        }
        return res;
    }
};

/**
 * BetterInfoCache
 */
BetterInfoCache* BetterInfoCache::sharedState() {
    static BetterInfoCache instance;
    return &instance;
}

BetterInfoCache::BetterInfoCache() {
    loadJson();
}

void BetterInfoCache::startLoading() {
    m_loadingStartAttempted = true;

    if(m_loadingStarted || !m_jsonLoaded) return;
    m_loadingStarted = true;

    saveJson();

    async::spawn([this] -> arc::Future<> {
        co_await cacheSavedLevels();
        co_await cacheFollowedCreators();
    }, [this] {
        log::debug("Finished initial caching");
        saveJson();
    });
}

void BetterInfoCache::loadJson() {
    arc::spawn([this] -> arc::Future<> {
        auto jsonOpt = utils::file::readJson(Mod::get()->getSaveDir() / "cache_v2.json");
        if(!jsonOpt) {
            log::error("Failed to read BetterInfoCache from json: {}", jsonOpt.unwrapErr());
            m_jsonLoaded = true;
            co_return;
        }

        auto json = jsonOpt.unwrap();

        auto userScoreJson = json["userScoreCache"];
        auto levelCache = json["levelCache"].as<std::unordered_map<int, CachedLevel>>().unwrapOrDefault();
        auto levelFailures = json["levelFailures"].as<std::unordered_map<int, int>>().unwrapOrDefault();
        auto levelDateCache = json["levelDateCache"].as<std::unordered_map<int, time_t>>().unwrapOrDefault();
        auto usernameCache = json["usernameCache"].as<std::unordered_map<int, std::string>>().unwrapOrDefault();
        auto levelDateStringCache = json["levelDateStringCache"].as<std::unordered_map<int, CachedLevelDate>>().unwrapOrDefault();

        co_await waitForMainThread([
            this, 
            levelCache = std::move(levelCache), 
            levelFailures = std::move(levelFailures), 
            levelDateCache = std::move(levelDateCache), 
            usernameCache = std::move(usernameCache),
            levelDateStringCache = std::move(levelDateStringCache),
            userScoreJson = std::move(userScoreJson)
        ]() {    
            m_userScoreCache = userScoreJson.as<std::unordered_map<int, Ref<GJUserScore>>>().unwrapOrDefault();
            m_levelCache = std::move(levelCache);
            m_levelFailures = std::move(levelFailures);
            m_levelDateCache = std::move(levelDateCache);
            m_levelDateStringCache = std::move(levelDateStringCache);
            m_usernameCache = std::move(usernameCache);
            m_jsonLoaded = true;

            if(m_loadingStartAttempted && !m_loadingStarted) {
                startLoading();
            }
        });

        log::debug("BetterInfoCache loaded from json");
    });
}

void BetterInfoCache::saveJson() {
    // this copies on purpose
    async::runtime().spawnBlocking<void>([
        userScoreCache = this->m_userScoreCache,
        m_levelCache = this->m_levelCache,
        m_levelFailures = this->m_levelFailures,
        m_levelDateCache = this->m_levelDateCache,
        m_usernameCache = this->m_usernameCache,
        m_levelDateStringCache = this->m_levelDateStringCache
    ] mutable {
        auto dict = GameLevelManager::sharedState()->m_onlineLevels->asExt<gd::string, GJGameLevel*>();
        std::erase_if(m_levelDateStringCache, [&dict](const auto& item) {
            auto idNum = fmt::to_string(item.first);
            if (!dict.contains(idNum)) {
                return true; 
            }
            return dict[idNum]->m_levelString.empty();
        });

        auto json = matjson::makeObject({
            {"userScoreCache", userScoreCache},
            {"levelCache", m_levelCache},
            {"levelFailures", m_levelFailures},
            {"levelDateCache", m_levelDateCache},
            {"usernameCache", m_usernameCache},
            {"levelDateStringCache", m_levelDateStringCache}
        });

        if(!utils::file::writeToJson(Mod::get()->getSaveDir() / "cache_v2.json", json)) {
            log::error("Failed to write BetterInfoCache to json");
        } else {
            log::debug("BetterInfoCache saved to json");
        }
    });
}

$on_mod(DataSaved) {
    BetterInfoCache::sharedState()->saveJson();
}

/**
 * User Score Caching
 */
arc::Future<> BetterInfoCache::cacheFollowedCreators() {
    log::debug("Started caching followed creators");
    std::vector<int> followedCreatorIDs;
    co_await waitForMainThread([this, &followedCreatorIDs] {
        auto followedCreators = GameLevelManager::sharedState()->m_followedCreators->asExt();
        for(auto [id, _] : followedCreators) {
            auto idNum = BetterInfo::stoi(id);
            if(m_userScoreCache.contains(idNum)) continue;

            followedCreatorIDs.push_back(idNum);
        }
    });

    for(auto id : followedCreatorIDs) {
        co_await arc::sleep(asp::Duration::fromSecs(5));
        co_await waitForMainThread([id] {
            log::debug("Caching score for followed creator with ID: {}", id);
            BetterInfoOnline::sharedState()->loadScores(id, false, nullptr);
        });
    }
}

void BetterInfoCache::cacheUserScore(int accountID, GJUserScore* score) {
    m_userScoreCache[accountID] = score;
}

void BetterInfoCache::cacheUserScores(CCArray* scores) {
    for(auto score : CCArrayExt<GJUserScore>(scores)) {
        if(!GameLevelManager::sharedState()->m_followedCreators->objectForKey(fmt::to_string(score->m_accountID))) continue;

        cacheUserScore(score->m_accountID, score);
    }
}

GJUserScore* BetterInfoCache::getUserScore(int accountID) {
    if(m_userScoreCache.contains(accountID)) {
        return m_userScoreCache[accountID];
    }

    auto score = GJUserScore::create();
    score->m_accountID = accountID;
    return score;
}

/**
 * Level Caching
 */
arc::Future<> BetterInfoCache::cacheSavedLevels() {
    co_await this->cacheLevels(GameLevelManager::sharedState()->m_dailyLevels);
    co_await this->cacheLevels(GameLevelManager::sharedState()->m_onlineLevels);
}

arc::Future<> BetterInfoCache::cacheLevels(CCDictionary* levels) {
    std::vector<int> needsDownloadingRated;
    std::vector<int> needsDownloadingUnrated;

    co_await waitForMainThread([this, levels, &needsDownloadingRated, &needsDownloadingUnrated] {
        for(auto [id, level] : levels->asExt<gd::string, GJGameLevel*>()) {
            auto idNum = level->m_levelID.value();
            if(m_levelCache.contains(idNum) || (m_levelFailures.contains(idNum) && m_levelFailures[idNum] > 5)) continue;

            if(!level->m_levelName.empty()) {
                cacheLevel(level);
            } else {
                if(level->m_stars > 0) {
                    needsDownloadingRated.push_back(idNum);
                } else {
                    needsDownloadingUnrated.push_back(idNum);
                }
            }
        }
    });

    log::debug("Caching levels, rated: {}, unrated: {}", needsDownloadingRated.size(), needsDownloadingUnrated.size());

    co_await cacheLevelBatch(std::move(needsDownloadingRated), true);
    co_await cacheLevelBatch(std::move(needsDownloadingUnrated), false);
}

arc::Future<> BetterInfoCache::cacheLevelBatch(std::vector<int> levelIDs, bool rated) {
    std::vector<int> batch;
    while (!levelIDs.empty()) {
        batch.clear();
        
        size_t batchSize = std::min(rated ? (size_t)1000 : (size_t)100, levelIDs.size());
        batch.reserve(batchSize);

        for (size_t i = 0; i < batchSize; i++) {
            batch.push_back(levelIDs.back());
            levelIDs.pop_back();
        }

        log::debug("Caching batch of levels, size: {}, rated: {}", batch.size(), rated);

        GJSearchObject* searchObject = nullptr;
        co_await waitForMainThread([&searchObject, batch, rated] {
            searchObject = GJSearchObject::create(
                rated ? SearchType::MapPackOnClick : SearchType::Type26, 
                fmt::format("{}", fmt::join(batch, ","))
            );
        });

        ServerUtils::getOnlineLevels(searchObject, [batch = std::move(batch), this] (std::shared_ptr<std::vector<Ref<GJGameLevel>>> levels, bool success, bool explicitError) mutable {
            for(auto& level : *levels) {
                cacheLevel(level);
                batch.erase(std::remove(batch.begin(), batch.end(), level->m_levelID.value()), batch.end());
            }

            for(auto id : batch) {
                log::trace("Failed to cache level with ID: {}", id);
                m_levelFailures[id]++;
            }

            log::debug("Cached batch of levels, size: {}", levels->size());
        });

        co_await arc::sleep(asp::Duration::fromSecs(30));
    }

    co_return;
}

void BetterInfoCache::cacheLevel(GJGameLevel* level) {
    m_levelCache[level->m_levelID] = {
        level->m_levelName,
        level->m_coins,
        level->m_demonDifficulty
    };
}

const BetterInfoCache::CachedLevel& BetterInfoCache::getLevel(int levelID) {
    if(m_levelCache.contains(levelID)) {
        return m_levelCache[levelID];
    }

    static CachedLevel emptyLevel = {"", 0, 0};
    return emptyLevel;
}

/**
 * Level Date Caching
 */
void BetterInfoCache::fetchLevelDate(int levelID, geode::Function<void(time_t)> callback) {
    if(ServerUtils::isGDPS()) return;
    if(m_levelDateCache.contains(levelID)) {
        callback(m_levelDateCache[levelID]);
        return;
    }

    async::spawn(
        ServerUtils::getBaseRequest().get(fmt::format("https://history.geometrydash.eu/api/v1/date/level/{}/", levelID)),
        [callback = std::move(callback), this](web::WebResponse response) mutable {
            if(!response.ok()) return;

            auto json = response.json().unwrapOrDefault();
            if(!json["approx"].contains("estimation") || !json["approx"].contains("online_id")) return;

            time_t time = TimeUtils::isoStringToTime(json["approx"]["estimation"].asString().unwrapOrDefault());
            m_levelDateCache[json["approx"]["online_id"].asInt().unwrapOrDefault()] = time;
            callback(time);
        }
    );
}

/**
 * Username Caching
 */
void BetterInfoCache::fetchUsername(int userID, geode::Function<void(std::string)> callback) {
    if(ServerUtils::isGDPS() || userID == 0) return;
    if(m_usernameCache.contains(userID)) {
        callback(m_usernameCache[userID]);
        return;
    }

    async::spawn(
        ServerUtils::getBaseRequest().get(fmt::format("https://history.geometrydash.eu/api/v1/user/{}/brief/", userID)),
        [callback = std::move(callback), this](web::WebResponse response) mutable {
            if(!response.ok()) return;

            auto json = response.json().unwrapOrDefault();
            if(!json.contains("username") || !json.contains("user_id")) return;

            std::string username = json["username"].asString().unwrapOrDefault();
            m_usernameCache[json["user_id"].asInt().unwrapOrDefault()] = username;
            callback(username);
        }
    );
}

std::string BetterInfoCache::tryGetUsername(int userID) {
    if(m_usernameCache.contains(userID)) return m_usernameCache[userID];

    fetchUsername(userID, [userID](std::string username) {
        log::debug("Fetched username for user ID {}: {}", userID, username);
    });

    return "";
}

/**
 * Rated List Caching
 */
void BetterInfoCache::cacheRatedListsFromMegaResponse(std::string_view response) {
    if(ServerUtils::isGDPS()) return;

    auto allLists = asp::iter::split(response, "#");
    auto allListsIt = allLists.next();
    if(!allListsIt) return;

    for(auto listStr : asp::iter::split(allListsIt.value(), "|")) {
        if (listStr.empty()) continue;
        m_levelLists.push_back(GJLevelList::create(BetterInfo::responseToDict(listStr)));
    }

    checkClaimableLists();
}

void BetterInfoCache::checkClaimableLists() {
    m_claimableLists.clear();

    for (const auto& list : m_levelLists) {
        if (GameStatsManager::get()->hasClaimedListReward(list)) continue;
        if (list->completedLevels() < list->m_levelsToClaim) continue;

        m_claimableLists.push_back(list);
    }
}

size_t BetterInfoCache::claimableListsCount() const {
    return m_claimableLists.size();
}

void BetterInfoCache::showClaimableLists() {
    if(m_claimableLists.empty()) return;

    auto searchObj = GJSearchObject::create((SearchType) 212156);
    searchObj->m_searchMode = 1;
    auto lists = CCArray::create();
    for(auto list : m_claimableLists) {
        if(list) lists->addObject(list);
    }
    GameLevelManager::sharedState()->storeSearchResult(lists, fmt::format("{}:{}:{}", lists->count(), 0, lists->count()), searchObj->getKey());

    auto scene = LevelBrowserLayer::scene(searchObj);
    auto transitionFade = CCTransitionFade::create(0.5, scene);
    CCDirector::sharedDirector()->pushScene(transitionFade);

    if(auto LBL = scene->getChildByType<LevelBrowserLayer>(0)) {
        LBL->m_refreshBtn->setVisible(false);
        if(auto starButton = LBL->getChildByIDRecursive("star-button"_spr)) {
            auto parent = starButton->getParent();
            starButton->removeFromParent();
            parent->updateLayout();
        }
    }
}

/**
 * Vault Code Caching
 */
void BetterInfoCache::cacheVaultCode(const std::string& id, std::string_view code) {
    m_vaultCodeCache[id] = code;
}

std::string BetterInfoCache::getVaultCode(const std::string& id) {
    if(m_vaultCodeCache.contains(id)) return m_vaultCodeCache[id];
    return "";
}

/**
 * Level String Date Caching
 */
void BetterInfoCache::cacheLevelDates(GJGameLevel* level) {
    m_levelDateStringCache[level->m_levelID] = {
        level->m_uploadDate,
        level->m_updateDate
    };
}

const BetterInfoCache::CachedLevelDate& BetterInfoCache::getLevelDates(int levelID) {
    if(m_levelDateStringCache.contains(levelID)) {
        return m_levelDateStringCache[levelID];
    }

    static CachedLevelDate emptyDate = {"", ""};
    return emptyDate;
}