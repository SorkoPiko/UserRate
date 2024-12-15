#include "AdminPage.hpp"

bool AdminPage::init() {
    const auto winSize = CCDirector::sharedDirector()->getWinSize();

    mainLayer = CCNode::create();
    mainLayer->setContentSize(winSize);

    background = CCSprite::create("GJ_gradientBG.png");
    background->setScale(winSize.width / background->getContentWidth());
    background->setScaleY(winSize.height / background->getContentHeight());
    background->setAnchorPoint({0, 0});
    background->setColor({0, 102, 255});
    background->setZOrder(-10);
    mainLayer->addChild(background);

    closeBtn = CCMenuItemExt::createSpriteExtra(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"), [this](CCObject*) {
            keyBackClicked();
        }
    );

    const auto closeMenu = CCMenu::create();
    closeMenu->addChild(closeBtn);
    closeMenu->setPosition({ 45, winSize.height - 25 });
    mainLayer->addChild(closeMenu);

    addSideArt(mainLayer, SideArt::Bottom);

    addChild(mainLayer);

    return true;
}

AdminPage* AdminPage::create() {
    const auto ret = new AdminPage();
    if (ret->init()) {
        ret->autorelease();g
        return ret;
    }
    delete ret;
    return nullptr;
}