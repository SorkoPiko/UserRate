#include "Global.hpp"

Global* Global::instance = nullptr;
std::mutex Global::mutex;

void Global::setLevelPage(const int page, const std::vector<matjson::Value> &levels) {
    if (levelPages.size() <= page) {
        levelPages.resize(page + 1);
    }
    levelPages[page] = levels;
}

std::vector<matjson::Value> Global::getLevelPage(const int page) const {
    if (levelPages.size() <= page) {
        return {};
    }
    return levelPages[page];
}
