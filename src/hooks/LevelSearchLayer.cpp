#include <Geode/modify/LevelSearchLayer.hpp>

#include "../managers/API.hpp"
#include "../managers/Global.hpp"

using namespace geode::prelude;

class $modify(RateLevelSearchLayer, LevelSearchLayer) {
    struct Fields {
        EventListener<web::WebTask> listener;
    };

    bool init(const int searchType) {
        if (!LevelSearchLayer::init(searchType)) return false;

        const auto menu = getChildByID("other-filter-menu");
        if (!menu) return true;

        const auto button = CCMenuItemSpriteExtra::create(
            [] {
                const auto sprite = CircleButtonSprite::create(
                    CCSprite::createWithSpriteFrameName("GJ_bigStar_noShadow_001.png"),
                    CircleBaseColor::Cyan,
                    CircleBaseSize::Medium
                );
                sprite->setScale(0.8f);
                return sprite;
            }(),
            this,
            menu_selector(RateLevelSearchLayer::onViewRates)
        );
        button->setID("rated-levels-button"_spr);

        menu->addChild(button);
        menu->updateLayout();

        return true;
    }

    void onViewRates(CCObject*) {
        API::getLatestRates([](const bool success) {
            if (success) {
                const auto scene = CCScene::create();
                const auto rates = Global::get()->getLatestRates();
                const auto browseLayer = LevelBrowserLayer::create(GJSearchObject::create(SearchType::Type19, fmt::format("{}", fmt::join(rates.begin(), rates.end(), ","))));
                scene->addChild(browseLayer);
                CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
            }
        }, m_fields->listener);
    }
};