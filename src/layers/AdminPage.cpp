#include "AdminPage.hpp"

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
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
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
    mainLayer->addChild(buttonMenu);

    title = CCLabelBMFont::create("Sent Levels", "bigFont.fnt");
    title->limitLabelWidth(275.f, 1.f, 0.1f);
    title->setPosition({winSize.width / 2, winSize.height - 25});

    listBG = CCLayerColor::create({ 0, 0, 0, 85 });
    listBG->ignoreAnchorPointForPosition(false);
    listBG->setAnchorPoint({ 0.5f, 0.5f });
    listBG->setPosition({winSize.width / 2, winSize.height / 2 - 10.f});
    listBG->setZOrder(-1);
    listBG->setID("list-bg");
    listBG->setContentSize({winSize.width / 1.5f, 220});
    mainLayer->addChild(listBG);

    createBorders();

    addSideArt(mainLayer, SideArt::Bottom);

    setKeypadEnabled(true);
    setKeyboardEnabled(true);
    addChild(mainLayer);

    return true;
}

void AdminPage::loadLevelPage() {

}

void AdminPage::onBack(CCObject*) {
    keyBackClicked();
}

void AdminPage::keyBackClicked() {
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, kPopTransitionFade);
}

// yes this is blatantly copied from Creation Rotation (https://github.com/TechStudent10/CreationRotation/blob/21cebb09b53752a90d7b7046b498376fb53a626d/src/layers/Lobby.cpp#L391)
void AdminPage::createBorders() const {
    const auto winSize = CCDirector::sharedDirector()->getWinSize();
    const float width = winSize.width / 1.5f;
    constexpr float height = 220.f;
    
    constexpr int SIDE_OFFSET = 7;
    constexpr int TOP_BOTTOM_OFFSET = 8;

    const auto topSide = CCSprite::createWithSpriteFrameName("GJ_table_side_001.png");
    topSide->setScaleY(
        listBG->getContentWidth() / topSide->getContentHeight()
    );
    topSide->setRotation(90.f);
    topSide->setPosition({
        width / 2,
        height + TOP_BOTTOM_OFFSET
    });
    topSide->setID("top-border");
    topSide->setZOrder(3);

    const auto bottomSide = CCSprite::createWithSpriteFrameName("GJ_table_side_001.png");
    bottomSide->setScaleY(
        listBG->getContentWidth() / bottomSide->getContentHeight()
    );
    bottomSide->setRotation(-90.f);
    bottomSide->setPosition({
        width / 2,
        0 - TOP_BOTTOM_OFFSET
    });
    bottomSide->setID("bottom-border");
    bottomSide->setZOrder(3);

    const auto leftSide = CCSprite::createWithSpriteFrameName("GJ_table_side_001.png");
    leftSide->setScaleY(
        (listBG->getContentHeight() + TOP_BOTTOM_OFFSET) / leftSide->getContentHeight()
    );
    leftSide->setPosition({
        -SIDE_OFFSET,
        height / 2
    });
    leftSide->setID("left-border");

    const auto rightSide = CCSprite::createWithSpriteFrameName("GJ_table_side_001.png");
    rightSide->setScaleY(
        (listBG->getContentHeight() + TOP_BOTTOM_OFFSET) / rightSide->getContentHeight()
    );
    rightSide->setRotation(180.f);
    rightSide->setPosition({
        width + SIDE_OFFSET,
        height / 2
    });
    rightSide->setID("right-border");

    listBG->addChild(topSide);
    listBG->addChild(bottomSide);
    listBG->addChild(leftSide);
    listBG->addChild(rightSide);


    const auto topLeftCorner = CCSprite::createWithSpriteFrameName("GJ_table_corner_001.png");
    topLeftCorner->setPosition({
        leftSide->getPositionX(), topSide->getPositionY()
    });
    topLeftCorner->setZOrder(2);
    topLeftCorner->setID("top-left-corner");

    auto topRightCorner = CCSprite::createWithSpriteFrameName("GJ_table_corner_001.png");
    topRightCorner->setFlipX(true);
    topRightCorner->setPosition({
        rightSide->getPositionX(), topSide->getPositionY()
    });
    topRightCorner->setZOrder(2);
    topRightCorner->setID("top-right-corner");

    auto bottomLeftCorner = CCSprite::createWithSpriteFrameName("GJ_table_corner_001.png");
    bottomLeftCorner->setFlipY(true);
    bottomLeftCorner->setPosition({
        leftSide->getPositionX(), bottomSide->getPositionY()
    });
    bottomLeftCorner->setZOrder(2);
    bottomLeftCorner->setID("bottom-left-corner");

    auto bottomRightCorner = CCSprite::createWithSpriteFrameName("GJ_table_corner_001.png");
    bottomRightCorner->setFlipX(true);
    bottomRightCorner->setFlipY(true);
    bottomRightCorner->setPosition({
        rightSide->getPositionX(), bottomSide->getPositionY()
    });
    bottomRightCorner->setZOrder(2);
    bottomRightCorner->setID("bottom-right-corner");

    listBG->addChild(topLeftCorner);
    listBG->addChild(topRightCorner);
    listBG->addChild(bottomLeftCorner);
    listBG->addChild(bottomRightCorner);
}
