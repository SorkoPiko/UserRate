#ifndef RATEDLEVEL_HPP
#define RATEDLEVEL_HPP

struct RatedLevel {
    int id{};
    int stars{};
    int feature{};
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

        return Ok(level);
    }

    static Value toJson(const RatedLevel& level) {
        Value value;

        value["stars"] = level.stars;
        value["feature"] = level.feature;

        return value;
    }
};

#endif
