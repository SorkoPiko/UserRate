#include <Geode/modify/RateStarsLayer.hpp>

#include "../managers/Global.hpp"

using namespace geode::prelude;

class $modify(RateRateStarsLayer, RateStarsLayer) {
    void onClose(CCObject* sender) {
        Global::get()->setCustomRating(false);
        RateStarsLayer::onClose(sender);
    }
};