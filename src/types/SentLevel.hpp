#ifndef SENTLEVEL_HPP
#define SENTLEVEL_HPP

using namespace geode::prelude;

struct SentLevel {
    int id{};

    std::unordered_map<int, int> featureSends;
    std::unordered_map<int, int> ratingSends;

    int totalSends{};
    float averageFeature{};
    float averageRating{};
};

template<>
struct matjson::Serialize<SentLevel> {
    static Result<SentLevel> fromJson(const Value& value) {
        SentLevel level;

        if (value["levelID"].isNumber()) {
            level.id = value["levelID"].asInt().unwrap();
        }

        float featureSends = 0;
        if (value["feature_spread"].isObject()) {
            float vals = 0;
            for (const auto& [key, val] : value["feature_spread"]) {
                const int keyNum = std::stoi(key);
                const int num = val.asInt().unwrap();
                level.featureSends[keyNum] = num;
                if (num == 0) continue;
                vals += keyNum * num;
                featureSends += num;
            }
            level.averageFeature = vals / featureSends;
        }

        float ratingSends = 0;
        if (value["stars_spread"].isObject()) {
            float vals = 0;
            for (const auto& [key, val] : value["stars_spread"]) {
                const int keyNum = std::stoi(key);
                const int num = val.asInt().unwrap();
                level.ratingSends[keyNum] = num;
                if (num == 0) continue;
                vals += keyNum * num;
                ratingSends += num;
            }
            level.averageRating = vals / ratingSends;
        }

        if (featureSends != ratingSends) {
            // something has gone terribly wrong
            log::error("Malformed sent level: feature sends ({}) != rating sends ({})", featureSends, ratingSends);
        }

        level.totalSends = featureSends;

        return Ok(level);
    }

    static Value toJson(const SentLevel& level) {
        Value value;

        value["levelID"] = level.id;

        Value featureSpread;
        for (const auto& [key, val] : level.featureSends) {
            featureSpread[std::to_string(key)] = val;
        }
        value["feature_spread"] = featureSpread;

        Value ratingSpread;
        for (const auto& [key, val] : level.ratingSends) {
            ratingSpread[std::to_string(key)] = val;
        }
        value["rating_spread"] = ratingSpread;

        return value;
    }
};

#endif
