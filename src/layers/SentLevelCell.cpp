#include "SentLevelCell.hpp"

#include <cmath>

#include "ViewSentLevelPopup.hpp"
#include "../managers/Global.hpp"

SentLevelCell* SentLevelCell::create(const SentLevel& level, const float width, AdminPage* adminPage) {
    const auto ret = new SentLevelCell();
    if (ret->init(level, width, adminPage)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool SentLevelCell::init(const SentLevel& level, float width, AdminPage* adminPage) {
    this->level = level;
    this->adminPage = adminPage;

    this->setContentSize({width, HEIGHT});

    const auto [name, is_platformer] = Global::get()->getLevel(level.id);

    nameLabel = CCLabelBMFont::create(!name.empty() ? name.c_str() : std::to_string(level.id).c_str(), "bigFont.fnt");
    nameLabel->setID("name-label");
    nameLabel->limitLabelWidth(250.f, 0.7f, 0.1f);
    nameLabel->setPosition({60.f, HEIGHT / 4.f * 3.f});
    nameLabel->setAnchorPoint({0.f, 0.5f});
    addChild(nameLabel);

    diffNode = DifficultyNode::create(level.averageRating, GJDifficultyName::Short, static_cast<GJFeatureState>(std::round(level.averageFeature)), is_platformer);
    diffNode->setID("difficulty-node");
    diffNode->setPosition({27.5f, HEIGHT / 2.f});
    addChild(diffNode);

    buttonMenu = CCMenu::create();
    buttonMenu->setID("button-menu");
    buttonMenu->setPosition({width - 30.f, HEIGHT / 2.f});
    addChild(buttonMenu);

    viewButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_infoBtn_001.png"),
        this,
        menu_selector(SentLevelCell::onView)
    );
    viewButton->setID("view-button");
    viewButton->setScale(0.8f);
    buttonMenu->addChild(viewButton);

    log::info("{}", matjson::Value(level).dump(0));

    return true;
}

void SentLevelCell::onView(CCObject*) {
    const auto popup = ViewSentLevelPopup::create(level, adminPage);
    popup->setZOrder(100);
    popup->show();
}