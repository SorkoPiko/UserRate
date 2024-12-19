#ifndef SENTLEVEL_HPP
#define SENTLEVEL_HPP

using namespace geode::prelude;

struct SentLevel {
    std::string name;
    int id{};

    std::vector<int> featureSends;
    std::vector<int> ratingSends;

    int totalSends{};
    int averageFeature{};
    int averageRating{};
};

template<>
struct matjson::Serialize<SentLevel> {
    static Result<SentLevel> fromJson(const Value& value) {
        SentLevel level;


    }
};

#endif
