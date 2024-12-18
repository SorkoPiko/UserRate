#include "AdminPage.hpp"

#include "SentPopup.hpp"

AdminPage* AdminPage::create() {
    const auto ret = new AdminPage();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

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

    closeMenu = CCMenu::create();
    closeMenu->setID("close-menu");
    closeMenu->setContentWidth(100.f);
    closeMenu->setAnchorPoint({ .0f, .5f });

    const auto closeBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png"),
        this,
        menu_selector(AdminPage::onBack)
    );
    closeBtn->setID("back-button");
    closeMenu->addChild(closeBtn);

    closeMenu->setLayout(
        RowLayout::create()
            ->setAxisAlignment(AxisAlignment::Start)
    );
    mainLayer->addChildAtPosition(closeMenu, Anchor::TopLeft, ccp(12, -25), false);

    buttonMenu = CCMenu::create();
    buttonMenu->setID("button-menu");

    const auto levelButton = CCMenuItemSpriteExtra::create(
        SearchButton::create("GJ_longBtn04_001.png", "Sent", 0.5, "GJ_sModIcon_001.png"),
        this,
        menu_selector(AdminPage::onSentLevels)
    );
    levelButton->setID("levels-button");
    buttonMenu->addChild(levelButton);

    mainLayer->addChildAtPosition(buttonMenu, Anchor::Center, ccp(0, 0), false);

    addSideArt(mainLayer, SideArt::Bottom);

    setKeypadEnabled(true);
    setKeyboardEnabled(true);
    addChild(mainLayer);

    return true;
}

void AdminPage::onSentLevels(CCObject*) {
    //CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, LevelBrowserLayer::scene(GJSearchObject::create(SearchType::MapPackOnClick, "102063008,111550678,113219586,113219585,113219584,113219583,113219582,113219581,113219580,113219579,113219578"))));
    SentPopup::create()->show();
}

void AdminPage::onBack(CCObject*) {
    keyBackClicked();
}

void AdminPage::keyBackClicked() {
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, kPopTransitionFade);
}