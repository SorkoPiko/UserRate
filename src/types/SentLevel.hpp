#ifndef SENTLEVEL_HPP
#define SENTLEVEL_HPP

using namespace geode::prelude;

struct SentLevel {
    int id{};

    std::unordered_map<int, int> featureSends;
    std::unordered_map<int, int> ratingSends;

    int totalSends{};
    int averageFeature{};
    int averageRating{};
};

template<>
struct matjson::Serialize<SentLevel> {
    static Result<SentLevel> fromJson(const Value& value) {
        SentLevel level;

        if (value["levelID"].isNumber()) {
            level.id = value["levelID"].asInt().unwrap();
        }

        int featureSends = 0;
        if (value["feature_spread"].isObject()) {
            int keys = 0;
            for (const auto& [key, val] : value["feature_spread"]) {
                keys++;
                const int num = val.asInt().unwrap();
                featureSends += num;
                level.featureSends[std::stoi(key)] = num;
            }
            level.averageFeature = featureSends / keys;
        }

        int ratingSends = 0;
        if (value["rating_spread"].isObject()) {
            int keys = 0;
            for (const auto& [key, val] : value["stars_spread"]) {
                keys++;
                const int num = val.asInt().unwrap();
                ratingSends += num;
                level.ratingSends[std::stoi(key)] = num;
            }
            level.averageRating = ratingSends / keys;
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
