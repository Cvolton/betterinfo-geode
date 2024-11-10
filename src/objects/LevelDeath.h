#pragma once

#include <time.h>

struct LevelDeath {
    public:
        int percentage;
        float x, y, rotation;
        time_t time = std::time(nullptr);
};

template <>
struct matjson::Serialize<LevelDeath> {
    static geode::Result<LevelDeath> fromJson(const matjson::Value& value) {
        return geode::Ok(LevelDeath {
            .percentage = (int) value["percentage"].asInt().unwrapOr(0),
            .x = (float) value["x"].asDouble().unwrapOr(0),
            .y = (float) value["y"].asDouble().unwrapOr(0),
            .rotation = (float) value["rotation"].asDouble().unwrapOr(0),
            .time = value["time"].as<time_t>().unwrapOr(0)
        });
    }
    static matjson::Value toJson(const LevelDeath& death) {
        return matjson::makeObject({
            { "percentage", death.percentage },
            { "x", death.x },
            { "y", death.y },
            { "rotation", death.rotation },
            { "time", death.time }
        });
    }
};