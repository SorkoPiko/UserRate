#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <mutex>

#include "../types/RatedLevel.hpp"
#include "../types/SentLevel.hpp"
#include "../types/SentLevelFilters.hpp"

class Global {
    static Global* instance;
    static std::mutex mutex;

    std::vector<int> modList{};
    std::vector<int> adminList{};

    std::vector<std::vector<SentLevel>> levelPages{};
    std::map<int, std::pair<std::string, bool>> levelCache{}; // levelID, level name, platformer

    std::map<int, std::optional<std::pair<int, int>>> levelRatingCache{}; // levelID, rating, feature
    std::map<int, DemonDifficulty> levelDemonRatingCache{}; // levelID, demon rating

    SentLevelFilters filters{};

    bool customRating = false;
    bool modRating = false;

    Global() = default;

public:
    Global(const Global&) = delete;
    Global& operator=(const Global&) = delete;

    static Global* get() {
        std::lock_guard lock(mutex);
        if (!instance) {
            instance = new Global();
        }
        return instance;
    }

    void setMods(const std::vector<int>& mods) {modList = mods;}
    void setAdmins(const std::vector<int>& admins) {adminList = admins;}

    [[nodiscard]] std::vector<int> getMods() const {return modList;}
    [[nodiscard]] std::vector<int> getAdmins() const {return adminList;}
    [[nodiscard]] bool isUserMod(const int accountID) const {return std::ranges::find(modList, accountID) != modList.end();}
    [[nodiscard]] bool isUserAdmin(const int accountID) const {return std::ranges::find(adminList, accountID) != adminList.end();}
    [[nodiscard]] bool isMod() const {return isUserMod(GJAccountManager::sharedState()->m_accountID) || isAdmin();}
    [[nodiscard]] bool isAdmin() const {return isUserAdmin(GJAccountManager::sharedState()->m_accountID);}

    void clearLevelPages() {levelPages.clear();}
    void setLevelPage(int page, const std::vector<SentLevel>& levels);
    [[nodiscard]] std::optional<std::vector<SentLevel>> getLevelPage(int page) const;

    void addLevel(const int levelID, const std::string& name, const bool isPlatformer) {levelCache[levelID] = {name, isPlatformer};}
    [[nodiscard]] std::pair<std::string, bool> getLevel(int levelID) const;
    [[nodiscard]] std::vector<int> allCached(const std::vector<int>& levelIDs) const;

    void setLevelRating(const int levelID, const std::optional<std::pair<int, int>>& value) {levelRatingCache[levelID] = value;}
    [[nodiscard]] bool levelRatingExists(const int levelID) const {return levelRatingCache.contains(levelID);}
    [[nodiscard]] std::optional<std::pair<int, int>> getLevelRating(int levelID) const;

    void setDemonRating(const int levelID, const DemonDifficulty value) {levelDemonRatingCache[levelID] = value;}
    [[nodiscard]] DemonDifficulty getDemonRating(int levelID) const;

    [[nodiscard]] SentLevelFilters getFilters() const {return filters;}
    void setFilters(const SentLevelFilters& value) {filters = value;}

    [[nodiscard]] bool isCustomRating() const {return customRating;}
    void setCustomRating(const bool value) {customRating = value;}

    [[nodiscard]] bool isModRating() const {return modRating;}
    void setModRating(const bool value) {modRating = value;}
};

#endif
