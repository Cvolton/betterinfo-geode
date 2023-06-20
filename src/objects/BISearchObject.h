#pragma once

struct BISearchObject {
    public:
        struct RangeItem {
            bool enabled = false;
            int min = 0;
            int max = 0;
        };

        std::set<int> difficulty;
        std::set<int> length;
        std::set<int> demonDifficulty;
        std::string str;
        bool star = false;
        bool noStar = false;
        bool uncompleted = false;
        bool uncompletedOrbs = false;
        bool uncompletedLeaderboard = false;
        bool uncompletedCoins = false;
        bool completed = false;
        bool completedOrbs = false;
        bool completedLeaderboard = false;
        bool completedCoins = false;
        RangeItem percentage;
        RangeItem percentageOrbs;
        RangeItem percentageLeaderboard;
        bool featured = false;
        bool original = false;
        bool twoPlayer = false;
        bool coins = false;
        bool noCoins = false;
        bool epic = false;
        int folder = 0;
        bool song = false;
        bool songCustom = false;
        int songID = 0;
        bool copied = false;
        bool downloaded = false;
        bool ldm = false;
        RangeItem idRange;
        bool copyable = false;
        bool freeCopy = false;
        bool unfeatured = false;
        bool unepic = false;
        RangeItem starRange;
        RangeItem gameVersion;
};