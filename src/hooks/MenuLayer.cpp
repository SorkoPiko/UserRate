#include <Geode/modify/MenuLayer.hpp>

#include "../layers/AdminPage.hpp"
#include "../managers/Global.hpp"

using namespace geode::prelude;

class $modify(RateMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        if (const auto menu = getChildByID("bottom-menu"); Global::getInstance()->isMod()) {
            const auto rateSprite = CircleButtonSprite::create(
                CCSprite::createWithSpriteFrameName("GJ_bigStar_noShadow_001.png"),
                CircleBaseColor::Cyan,
                CircleBaseSize::MediumAlt
            );
            const auto rateButton = CCMenuItemSpriteExtra::create(
                rateSprite,
                this,
                menu_selector(RateMenuLayer::onAdmin)
            );
            rateButton->setID("admin-page"_spr);
            menu->addChild(rateButton);
            menu->updateLayout();
        }

        return true;
    }

    void onAdmin(CCObject*) {
        const auto layer = AdminPage::create();
        const auto scene = CCScene::create();
        scene->addChild(layer);
        CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
    }
};