#include <Geode/modify/LevelInfoLayer.hpp>

#include "../managers/Global.hpp"
#include "../nodes/DifficultyNode.hpp"

using namespace geode::prelude;

class $modify(RateLevelInfoLayer, LevelInfoLayer) {
    bool init(GJGameLevel* level, const bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        const auto global = Global::get();
        const auto rating = global->getLevelRating(m_level->m_levelID.value());

        if (rating.has_value() && m_level->m_stars == 0) {
            m_difficultySprite->updateDifficultyFrame(DifficultyNode::getDifficultyForStars(rating.value().first), GJDifficultyName::Short);
            m_difficultySprite->updateFeatureState(static_cast<GJFeatureState>(rating.value().second));
        }

        CCNode* menu;
        if (rating.has_value() && rating.value().first == 10) menu = getChildByID("right-side-menu");
        else if (!rating.has_value() && m_level->m_stars == 0) {
            menu = getChildByID("left-side-menu");
            if (global->isMod()) {
                const auto rateSprite = CircleButtonSprite::create(
                    CCSprite::createWithSpriteFrameName("modBadge_01_001.png"),
                    CircleBaseColor::Cyan
                );
                const auto rateButton = CCMenuItemSpriteExtra::create(
                    rateSprite,
                    this,
                    menu_selector(RateLevelInfoLayer::onModRate)
                );
                rateButton->setID("rate-stars-mod"_spr);
                menu->addChild(rateButton);
                menu->updateLayout();
            }
        }
        else return true;

        const auto rateSprite = CircleButtonSprite::create(
            CCSprite::createWithSpriteFrameName(m_level->m_demon == 1 ? "diffIcon_06_btn_001.png" : "GJ_bigStar_noShadow_001.png"),
            CircleBaseColor::Cyan
        );

        const auto button = CCMenuItemSpriteExtra::create(
            rateSprite,
            this,
            m_level->m_demon == 1 ? menu_selector(RateLevelInfoLayer::onCustomDemonRate) : menu_selector(RateLevelInfoLayer::onCustomRate)
        );
        button->setID("rate-stars"_spr);
        menu->addChild(button);
        menu->updateLayout();
        return true;
    }

    void onRateStars(CCObject* sender) {
        const auto global = Global::get();
        global->setCustomRating(false);
        global->setModRating(false);
        LevelInfoLayer::onRateStars(sender);
    }

    void onRateStarsMod(CCObject* sender) {
        const auto global = Global::get();
        global->setCustomRating(false);
        global->setModRating(false);
        LevelInfoLayer::onRateStarsMod(sender);
    }

    void onRateDemon(CCObject* sender) {
        Global::get()->setCustomRating(false);
        LevelInfoLayer::onRateDemon(sender);
    }

    void onCustomRate(CCObject*) {
        Global::get()->setCustomRating(true);

        const auto layer = RateStarsLayer::create(m_level->m_levelID.value(), m_level->isPlatformer(), true);
        layer->show();
    }

    void onModRate(CCObject*) {
        Global::get()->setModRating(true);

        const auto layer = RateStarsLayer::create(m_level->m_levelID.value(), m_level->isPlatformer(), true);
        layer->show();
    }

    void onCustomDemonRate(CCObject*) {
        Global::get()->setCustomRating(true);

        const auto layer = RateDemonLayer::create(this->m_level->m_levelID.value());
        layer->m_delegate = this;
        layer->show();
    }
};