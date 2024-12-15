#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <mutex>

class Global {
    static Global* instance;
    static std::mutex mutex;

    bool checkedModStatus = false;
    bool mod = true;
    bool admin = true;
    bool customRating = false;

    void checkAccountStatus();

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

    bool isMod() {
        checkAccountStatus();
        return mod || admin;
    }

    bool isAdmin() {
        checkAccountStatus();
        return admin;
    }

    [[nodiscard]] bool isCustomRating() const {return customRating;}
    void setCustomRating(const bool value) {customRating = value;}
};

#endif
