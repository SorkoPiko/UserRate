#include <Geode/modify/GameLevelManager.hpp>

#include "../managers/Global.hpp"

using namespace geode::prelude;

class $modify(RateGameLevelManager, GameLevelManager) {
    bool hasRatedDemon(const int id) {
        if (Global::get()->isCustomRating()) return false;
        return GameLevelManager::hasRatedDemon(id);
    }

    // void markLevelAsRatedDemon(const int id) {
    //     if (Global::get()->isCustomRating) return;
    //     GameLevelManager::markLevelAsRatedDemon(id);
    // }
};