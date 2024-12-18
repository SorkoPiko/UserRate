#include "Global.hpp"

#include "API.hpp"

Global* Global::instance = nullptr;
std::mutex Global::mutex;

void Global::checkAccountStatus() {
    const auto accountID = GJAccountManager::sharedState()->m_accountID;

    if (isUserChecked(accountID)) return;

    API::isModerator(accountID, [this, accountID](const bool mod, const bool admin, const std::string& message) {
        if (!message.empty()) {
            FLAlertLayer::create(
                "Error",
                fmt::format("Failed to check moderator status: {}.", message),
                "OK"
            )->show();
        }
        this->mod = mod;
        if (mod) addMod(accountID);

        this->admin = admin;
        if (admin) addAdmin(accountID);

        addChecked(accountID);
    });
}

void Global::clearUser(const int accountID) {
    if (const auto modIt = std::ranges::find(modList, accountID); modIt != modList.end()) {
        modList.erase(modIt);
    }

    if (const auto adminIt = std::ranges::find(adminList, accountID); adminIt != adminList.end()) {
        adminList.erase(adminIt);
    }

    if (const auto checkedIt = std::ranges::find(checkedList, accountID); checkedIt != checkedList.end()) {
        checkedList.erase(checkedIt);
    }
}
