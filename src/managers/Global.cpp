#include "Global.hpp"

Global* Global::instance = nullptr;
std::mutex Global::mutex;

void Global::checkAccountStatus() {
    if (checkedModStatus) return;
    // Check account status
    mod = true;
    admin = true;
    checkedModStatus = true;
}

