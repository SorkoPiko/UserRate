#include <Geode/modify/RateStarsLayer.hpp>

#include "../managers/API.hpp"
#include "../managers/Global.hpp"

using namespace geode::prelude;

class $modify(RateRateStarsLayer, RateStarsLayer) {
    void onRate(CCObject* sender) {
        const auto global = Global::get();
        if (!global->isModRating()) return RateStarsLayer::onRate(sender);

        API::rateLevel(m_levelID, m_starsRate, m_featureState, [this, global](const bool success) {
            if (success) {
                FLAlertLayer::create(
                    "Success",
                    "Level rated successfully.",
                    "OK"
                )->show();
                global->clearLevelPages();
                RateStarsLayer::onClose(nullptr);
            }
        });
    }

    void onClose(CCObject* sender) {
        Global::get()->setCustomRating(false);
        Global::get()->setModRating(false);
        RateStarsLayer::onClose(sender);
    }
};