#include <Geode/modify/LevelInfoLayer.hpp>

#include "../managers/API.hpp"
#include "../managers/Global.hpp"
#include "../nodes/DifficultyNode.hpp"

using namespace geode::prelude;

class $modify(RateLevelInfoLayer, LevelInfoLayer) {
    struct Fields {
        CCMenuItemSpriteExtra* rateButton = nullptr;
    };

    bool init(GJGameLevel* level, const bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        if (const auto global = Global::get(); !global->levelRatingExists(m_level->m_levelID.value())) {
            API::checkRatedLevels({m_level->m_levelID.value()}, [this](const bool success) {
                if (success) addCustom();
            });
        }
        else addCustom();

        return true;
    }

    void onBack(CCObject* sender) {
        LevelInfoLayer::onBack(sender);
    }

    void addCustom() {
        const auto global = Global::get();
        const auto rating = global->getLevelRating(m_level->m_levelID.value());

        modifySprite();

        const auto leftMenu = getChildByID("left-side-menu");
        const auto rightMenu = getChildByID("right-side-menu");
        if (!leftMenu || !rightMenu) return;

        if (rating.has_value() && global->isAdmin()) {
            if (global->isMod()) {
                const auto derateSprite = CircleButtonSprite::create(
                    CCSprite::createWithSpriteFrameName("modBadge_01_001.png"),
                    CircleBaseColor::DarkPurple
                );
                m_fields->rateButton = CCMenuItemSpriteExtra::create(
                    derateSprite,
                    this,
                    menu_selector(RateLevelInfoLayer::onDerate)
                );
                m_fields->rateButton->setID("derate-level"_spr);
                leftMenu->addChild(m_fields->rateButton);
                leftMenu->updateLayout();
            }
        }

        CCNode* menu;
        if (rating.has_value() && rating.value().first == 10) menu = rightMenu;
        else if (!rating.has_value() && m_level->m_stars == 0) {
            menu = leftMenu;
            if (global->isMod()) {
                const auto rateSprite = CircleButtonSprite::create(
                    CCSprite::createWithSpriteFrameName("modBadge_01_001.png"),
                    CircleBaseColor::Cyan
                );
                m_fields->rateButton = CCMenuItemSpriteExtra::create(
                    rateSprite,
                    this,
                    menu_selector(RateLevelInfoLayer::onModRate)
                );
                m_fields->rateButton->setID("rate-level"_spr);
                menu->addChild(m_fields->rateButton);
                menu->updateLayout();
            }
        }
        else return;

        const auto demon = rating.has_value() && rating.value().first == 10;

        const auto rateSprite = CircleButtonSprite::create(
            CCSprite::createWithSpriteFrameName(demon ? "diffIcon_06_btn_001.png" : "GJ_bigStar_noShadow_001.png"),
            CircleBaseColor::Cyan
        );

        const auto button = CCMenuItemSpriteExtra::create(
            rateSprite,
            this,
            demon ? menu_selector(RateLevelInfoLayer::onCustomDemonRate) : menu_selector(RateLevelInfoLayer::onCustomRate)
        );
        button->setID("rate-stars"_spr);
        menu->addChild(button);
        menu->updateLayout();
    }

    void modifySprite() const {
        const auto global = Global::get();
        if (const auto rating = global->getLevelRating(m_level->m_levelID.value()); rating.has_value() && m_level->m_stars == 0) {
            int diff = DifficultyNode::getDifficultyForStars(rating.value().first);
            if (rating.value().first == 10) diff = DifficultyNode::getDifficultyForDemon(global->getDemonRating(m_level->m_levelID.value()));
            m_difficultySprite->updateDifficultyFrame(diff, GJDifficultyName::Short);
            m_difficultySprite->updateFeatureState(static_cast<GJFeatureState>(rating.value().second));
        }
    }

    void levelDownloadFinished(GJGameLevel* p0) {
        LevelInfoLayer::levelDownloadFinished(p0);
        modifySprite();
    }

    void levelUpdateFinished(GJGameLevel* p0, const UpdateResponse p1) {
        LevelInfoLayer::levelUpdateFinished(p0, p1);
        modifySprite();
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

        FLAlertLayer::create(
            "Rating guidelines",
            "1. The level must be at least <cr>40 seconds</c> long (there are exclusions, contact an <cg>admin</c> if you believe you found one).\n"
            "2. Don't <cr>spam ratings</c> (i.e. rating <cy>all your own levels</c>, rating <co>all your friend's levels</c>, etc).\n"
            "3. The level <cb>must</c> be at least <cr>partially decorated</c> (no layouts, etc). It doesn't need to be <cg>amazing decoration</c>, but <cy>effort must be present</c>.",
            "OK"
        )->show();
    }

    void onDerate(CCObject* sender) {
        createQuickPopup(
            "Derate level",
            "Are you sure you want to derate this level?",
            "Cancel", "Derate",
            [this](auto, const bool btn2) {
                if (btn2) {
                    API::derateLevel(m_level->m_levelID.value(), [this](const bool success) {
                        if (success) {
                            FLAlertLayer::create(
                                "Derate level",
                                "Success! The level has been derated.",
                                "OK"
                            )->show();

                            if (m_fields->rateButton) m_fields->rateButton->removeFromParentAndCleanup(true);
                        }
                    });
                }
            }
        )->m_button1->updateBGImage("GJ_button_06.png");
    }

    void onCustomDemonRate(CCObject*) {
        Global::get()->setCustomRating(true);

        const auto layer = RateDemonLayer::create(this->m_level->m_levelID.value());
        layer->m_delegate = this;
        layer->show();
    }
};