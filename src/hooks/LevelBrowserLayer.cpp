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
    struct Fields {
        bool block = false;
        EventListener<web::WebTask> listener;
    };
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

        if (levelIds.empty()) return modifyDifficultySprites();
        m_fields->block = true;
        API::checkRatedLevels(levelIds, [this](const bool success) {
            if (success) modifyDifficultySprites();
            else m_fields->block = false;
        }, m_fields->listener);
    }

    // void keyDown(const enumKeyCodes p0) override {
    //     if (m_fields->block) return;
    //     LevelBrowserLayer::keyDown(p0);
    // }
    //
    // void onBack(CCObject* sender) override {
    //     if (m_fields->block) return;
    //     LevelBrowserLayer::onBack(sender);
    // }
    //
    // void keyBackClicked() override {
    //     if (m_fields->block) return;
    //     LevelBrowserLayer::keyBackClicked();
    // }

    void modifyDifficultySprites() {
        const auto global = Global::get();

        if (
            !m_list ||
            !m_list->m_listView ||
            !m_list->m_listView->m_tableView ||
            !m_list->m_listView->m_tableView->m_contentLayer
        ) return;

        auto* children = m_list->m_listView->m_tableView->m_contentLayer->getChildren();

        if (!children) return;

        for (auto* cell_ : CCArrayExt<CCNode*>(children)) {
            if (!cell_) continue;
            const auto* cell = typeinfo_cast<LevelCell*>(cell_);
            if (!cell || !cell->m_level || !cell->m_mainLayer) continue;
            if (!isValidLevelType(cell->m_level->m_levelType)) continue;

            const auto rating = global->getLevelRating(cell->m_level->m_levelID.value());
            if (!rating.has_value()) continue;

            const auto sprite = typeinfo_cast<GJDifficultySprite*>(cell->m_mainLayer->getChildByIDRecursive("difficulty-sprite"));
            if (!sprite) continue;

            int diff = DifficultyNode::getDifficultyForStars(rating.value().first);
            if (rating.value().first == 10) diff = DifficultyNode::getDifficultyForDemon(global->getDemonRating(cell->m_level->m_levelID.value()));
            sprite->updateDifficultyFrame(diff, GJDifficultyName::Short);
            sprite->updateFeatureState(static_cast<GJFeatureState>(rating.value().second));
        }

        m_fields->block = false;
    }
};