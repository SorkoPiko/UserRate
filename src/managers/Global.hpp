#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <mutex>

class Global {
    static Global* instance;
    static std::mutex mutex;

    Global() {}

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

    bool checkedModStatus = false;
    bool isMod = true;
    bool isCustomRating = false;
};

#endif
