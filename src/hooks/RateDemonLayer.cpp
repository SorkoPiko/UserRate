#include <Geode/modify/RateDemonLayer.hpp>

#include "../managers/Global.hpp"

using namespace geode::prelude;

class $modify(RateRateDemonLayer, RateDemonLayer) {
    void onClose(CCObject* sender) {
        Global::getInstance()->setCustomRating(false);
        RateDemonLayer::onClose(sender);
    }
};