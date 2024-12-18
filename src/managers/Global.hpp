#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#define SERVER_URL "http://127.0.0.1:8000"

#include <mutex>

class Global {
    static Global* instance;
    static std::mutex mutex;

    std::vector<int> modList{};
    std::vector<int> adminList{};
    std::vector<int> checkedList{};

    bool checkedModStatus = false;
    bool mod = true;
    bool admin = true;
    bool customRating = false;

    Global() = default;

public:
    Global(const Global&) = delete;
    Global& operator=(const Global&) = delete;

    static Global* getInstance() {
        std::lock_guard lock(mutex);
        if (!instance) {
            instance = new Global();
        }
        return instance;
    }

    void checkAccountStatus();

    void clearUser(int accountID);

    [[nodiscard]] bool isUserMod(const int accountID) const {return std::ranges::find(modList, accountID) != modList.end();}
    void addMod(const int accountID) {modList.push_back(accountID);}

    [[nodiscard]] bool isUserAdmin(const int accountID) const {return std::ranges::find(adminList, accountID) != adminList.end();}
    void addAdmin(const int accountID) {adminList.push_back(accountID);}

    [[nodiscard]] bool isUserChecked(const int accountID) const {return std::ranges::find(checkedList, accountID) != checkedList.end();}
    void addChecked(const int accountID) {checkedList.push_back(accountID);}

    [[nodiscard]] bool isMod() const {return mod || admin;}
    [[nodiscard]] bool isAdmin() const {return admin;}
    [[nodiscard]] bool isCustomRating() const {return customRating;}
    void setCustomRating(const bool value) {customRating = value;}
};

#endif
