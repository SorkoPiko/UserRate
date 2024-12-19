#include <Geode/modify/RateDemonLayer.hpp>

#include "../managers/Global.hpp"

using namespace geode::prelude;

class $modify(RateRateDemonLayer, RateDemonLayer) {
    void onClose(CCObject* sender) {
        Global::get()->setCustomRating(false);
        RateDemonLayer::onClose(sender);
    }
};