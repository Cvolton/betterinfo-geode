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
    static LevelDeath from_json(const matjson::Value& value) {
        return LevelDeath {
            .percentage = value["percentage"].as_int(),
            .x = (float) value["x"].as_double(),
            .y = (float) value["y"].as_double(),
            .rotation = (float) value["rotation"].as_double(),
            .time = value["time"].as_int()
        };
    }
    static matjson::Value to_json(const LevelDeath& death) {
        return matjson::Object {
            { "percentage", death.percentage },
            { "x", death.x },
            { "y", death.y },
            { "rotation", death.rotation },
            { "time", death.time }
        };
    }
};