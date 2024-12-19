#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#define SERVER_URL "http://127.0.0.1:8000"

#include <mutex>

#include "../types/SentLevel.hpp"

class Global {
    static Global* instance;
    static std::mutex mutex;

    std::vector<int> modList{};
    std::vector<int> adminList{};

    std::vector<std::vector<SentLevel>> levelPages{};
    std::map<int, std::pair<std::string, bool>> levelCache{}; // levelID, level name, platformer

    bool customRating = false;

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
    [[nodiscard]] std::vector<SentLevel> getLevelPage(int page) const;

    void addLevel(const int levelID, const std::string& name, const bool isPlatformer) {levelCache[levelID] = {name, isPlatformer};}
    [[nodiscard]] std::pair<std::string, bool> getLevel(int levelID) const;
    std::vector<int> allCached(const std::vector<int>& levelIDs) const;

    [[nodiscard]] bool isCustomRating() const {return customRating;}
    void setCustomRating(const bool value) {customRating = value;}
};

#endif
