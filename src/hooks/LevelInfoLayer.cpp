#include <Geode/modify/LevelInfoLayer.hpp>

#include "../managers/Global.hpp"

using namespace geode::prelude;

class $modify(RateLevelInfoLayer, LevelInfoLayer) {
    bool init(GJGameLevel* level, const bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) {
            return false;
        }

        if (!Global::getInstance()->isMod() && m_level->m_stars == 0) return true;

        CCNode* menu;
        if (m_level->m_stars == 0) {
            menu = getChildByID("left-side-menu");
        } else if (m_level->m_demon == 1) {
            menu = getChildByID("right-side-menu");
        } else return true;

        const auto rateSprite = CircleButtonSprite::create(
            CCSprite::createWithSpriteFrameName(m_level->m_demon == 1 ? "diffIcon_06_btn_001.png" : "GJ_bigStar_noShadow_001.png"),
            CircleBaseColor::Cyan
        );

        const auto button = CCMenuItemSpriteExtra::create(
            rateSprite,
            this,
            m_level->m_demon == 1 ? menu_selector(RateLevelInfoLayer::onCustomDemonRate) : menu_selector(RateLevelInfoLayer::onCustomRate)
        );

        menu->addChild(button);
        menu->updateLayout();
        return true;
    }

    void onRateStarsMod(CCObject* sender) {
        Global::getInstance()->setCustomRating(false);
        LevelInfoLayer::onRateStarsMod(sender);
    }

    void onRateDemon(CCObject* sender) {
        Global::getInstance()->setCustomRating(false);
        LevelInfoLayer::onRateDemon(sender);
    }

    void onCustomRate(CCObject*) {
        log::info("custom rate");
        Global::getInstance()->setCustomRating(true);

        const auto layer = RateStarsLayer::create(m_level->m_levelID.value(), m_level->isPlatformer(), true);
        layer->show();
    }

    void onCustomDemonRate(CCObject*) {
        log::info("custom demon rate");
        Global::getInstance()->setCustomRating(true);

        const auto layer = RateDemonLayer::create(this->m_level->m_levelID.value());
        layer->m_delegate = this;
        layer->show();
    }
};