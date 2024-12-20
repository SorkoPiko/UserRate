#ifndef SENTLEVELFILTERS_HPP
#define SENTLEVELFILTERS_HPP

enum SentLevelSearchType {
    TOP = 0,
    RECENT = 1,
    OLDEST = 2
};

struct SentLevelFilters {
    SentLevelSearchType sort = TOP;
    int page = 0;

    int minSends = 0;
    std::optional<int> maxSends;

    int minAvgStars = 1;
    int maxAvgStars = 10;

    int minAvgFeature = 0;
    int maxAvgFeature = 4;

    bool operator==(const SentLevelFilters& other) const {
    return sort == other.sort &&
        page == other.page &&
        minSends == other.minSends &&
        maxSends == other.maxSends &&
        minAvgStars == other.minAvgStars &&
        maxAvgStars == other.maxAvgStars &&
        minAvgFeature == other.minAvgFeature &&
        maxAvgFeature == other.maxAvgFeature;

    }

    bool operator!=(const SentLevelFilters& other) const {
        return !(*this == other);
    }

    static const char* sortToString(const SentLevelSearchType type) {
        switch (type) {
            case TOP: return "Top";
            case RECENT: return "Recent";
            case OLDEST: return "Oldest";
            default: return "Unknown";
        }
    }
};

#endif
