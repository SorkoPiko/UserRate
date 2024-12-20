#include <Geode/modify/LevelBrowserLayer.hpp>

#include "../managers/API.hpp"
#include "../managers/Global.hpp"
#include "../nodes/DifficultyNode.hpp"

using namespace geode::prelude;

// ???
// idk what this is as GJLevelType doesn't even have a 4th value
// keeping it here just in case
// https://github.com/GlobedGD/globed2/blob/f83f3e9413b6ca5d41a80ed98edfb62228bb84f3/src/hooks/level_browser_layer.hpp#L19
constexpr bool isValidLevelType(GJLevelType level) {
    return static_cast<int>(level) == 3 || static_cast<int>(level) == 4;
}

class $modify(RateLevelBrowserLayer, LevelBrowserLayer) {
    void setupLevelBrowser(CCArray* levels) {
        LevelBrowserLayer::setupLevelBrowser(levels);
        if (!m_list->m_listView) {
            return;
        }

        const auto global = Global::get();

        // thanks dankmeme01
        // https://github.com/GlobedGD/globed2/blob/f83f3e9413b6ca5d41a80ed98edfb62228bb84f3/src/hooks/level_browser_layer.cpp#L11
        std::vector<int> levelIds;
        for (auto* level_ : CCArrayExt<CCObject*>(levels)) {
            if (!typeinfo_cast<GJGameLevel*>(level_)) continue;
            if (auto* level = dynamic_cast<GJGameLevel*>(level_); isValidLevelType(level->m_levelType) && level->m_stars.value() == 0 && !global->levelRatingExists(level->m_levelID)) levelIds.push_back(level->m_levelID);
        }

        if (levelIds.empty()) return modifyDifficultySprite();
        API::checkRatedLevels(levelIds, [this](const bool success) {
            if (success) modifyDifficultySprite();
        });
    }

    void modifyDifficultySprite() {
        const auto global = Global::get();

        for (auto* cell_ : CCArrayExt<CCNode*>(m_list->m_listView->m_tableView->m_contentLayer->getChildren())) {
            const auto* cell = typeinfo_cast<LevelCell*>(cell_);
            if (!cell) {
                continue;
            }

            if (!isValidLevelType(cell->m_level->m_levelType)) {
                continue;
            }

            const auto rating = global->getLevelRating(cell->m_level->m_levelID.value());
            if (!rating.has_value()) continue;
            const auto sprite = typeinfo_cast<GJDifficultySprite*>(cell->m_mainLayer->getChildByIDRecursive("difficulty-sprite"));
            if (!sprite) continue;
            sprite->updateDifficultyFrame(DifficultyNode::getDifficultyForStars(rating.value().first), GJDifficultyName::Short);
            sprite->updateFeatureState(static_cast<GJFeatureState>(rating.value().second));
        }
    }
};