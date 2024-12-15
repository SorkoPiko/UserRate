#include <Geode/modify/MenuLayer.hpp>

#include "../layers/AdminPage.hpp"
#include "../managers/Global.hpp"

using namespace geode::prelude;

class $modify(RateMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        if (const auto menu = getChildByID("bottom-menu"); menu && Global::getInstance()->isAdmin()) {
            const auto rateButton = CCMenuItemSpriteExtra::create(
                CCSprite::createWithSpriteFrameName("GJ_rateBtn_001.png"),
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