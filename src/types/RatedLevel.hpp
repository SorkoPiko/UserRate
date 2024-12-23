#ifndef RATEDLEVEL_HPP
#define RATEDLEVEL_HPP

using namespace geode::prelude;

enum DemonDifficulty {
    EASY = 1,
    MEDIUM = 2,
    HARD = 3,
    INSANE = 4,
    EXTREME = 5
};

struct RatedLevel {
    int id{};
    int stars{};
    int feature{};
    std::optional<DemonDifficulty> avgDemonRating = std::nullopt;
};

template<>
struct matjson::Serialize<RatedLevel> {
    static Result<RatedLevel> fromJson(const Value& value) {
        RatedLevel level;

        if (value["levelID"].isNumber()) {
            level.id = value["levelID"].asInt().unwrap();
        }

        if (value["stars"].isNumber()) {
            level.stars = value["stars"].asInt().unwrap();
        }

        if (value["feature"].isNumber()) {
            level.feature = value["feature"].asInt().unwrap();
        }

        if (value["avgDemonRating"].isNumber()) {
            level.avgDemonRating = static_cast<DemonDifficulty>(value["avgDemonRating"].asInt().unwrap());
        }

        return Ok(level);
    }

    static Value toJson(const RatedLevel& level) {
        Value value;

        value["levelID"] = level.id;
        value["stars"] = level.stars;
        value["feature"] = level.feature;
        if (level.avgDemonRating.has_value()) value["avgDemonRating"] = static_cast<int>(level.avgDemonRating.value());

        return value;
    }
};

#endif
