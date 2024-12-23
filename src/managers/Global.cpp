#include "Global.hpp"

Global* Global::instance = nullptr;
std::mutex Global::mutex;

void Global::setLevelPage(const int page, const std::vector<SentLevel> &levels) {
    if (levelPages.size() <= page) levelPages.resize(page + 1);
    levelPages[page] = levels;
}

std::optional<std::vector<SentLevel>> Global::getLevelPage(const int page) const {
    if (levelPages.size() <= page) return std::nullopt;
    return levelPages[page];
}

std::pair<std::string, bool> Global::getLevel(const int levelID) const {
    if (const auto it = levelCache.find(levelID); it != levelCache.end()) return it->second;
    return {"", false};
}

std::vector<int> Global::allCached(const std::vector<int>& levelIDs) const {
    std::vector<int> notCached;
    for (const int id : levelIDs) if (!levelCache.contains(id)) notCached.push_back(id);
    return notCached;
}

std::optional<std::pair<int, int>> Global::getLevelRating(const int levelID) const {
    if (const auto it = levelRatingCache.find(levelID); it != levelRatingCache.end()) return it->second;
    return std::nullopt;
}

DemonDifficulty Global::getDemonRating(const int levelID) const {
    if (const auto it = levelDemonRatingCache.find(levelID); it != levelDemonRatingCache.end()) return it->second;
    return HARD;
}
