#include "Global.hpp"

Global* Global::instance = nullptr;
std::mutex Global::mutex;

void Global::setLevelPage(const int page, const std::vector<SentLevel> &levels) {
    if (levelPages.size() <= page) levelPages.resize(page + 1);
    levelPages[page] = levels;
}

std::vector<SentLevel> Global::getLevelPage(const int page) const {
    if (levelPages.size() <= page) return {};
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