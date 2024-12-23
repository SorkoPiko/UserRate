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

std::string SentLevelCell::floatToString(const float number, const int precision) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << number;
    return stream.str();
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

    diffNode = DifficultyNode::create(std::round(level.averageRating), GJDifficultyName::Short, static_cast<GJFeatureState>(std::round(level.averageFeature)), is_platformer);
    diffNode->setID("difficulty-node");
    diffNode->setPosition({27.5f, HEIGHT / 2.f});
    addChild(diffNode);

    sendsSprite = CCSprite::createWithSpriteFrameName("GJ_sDownloadIcon_001.png");
    sendsSprite->setID("sends-sprite");
    sendsSprite->setPosition({(width - 100.f)/3.f - 30.f, HEIGHT / 3.f});
    addChild(sendsSprite);

    sendsLabel = CCLabelBMFont::create(numToAbbreviatedString(level.totalSends).c_str(), "bigFont.fnt");
    sendsLabel->setID("sends-label");
    sendsLabel->setAnchorPoint({0.f, 0.5f});
    sendsLabel->setPosition({(width - 100.f)/3.f - 20.f, HEIGHT / 3.f});
    sendsLabel->limitLabelWidth(100.f, 0.5f, .1f);
    addChild(sendsLabel);

    starsSprite = CCSprite::createWithSpriteFrameName(is_platformer ? "GJ_moonsIcon_001.png" : "GJ_sStarsIcon_001.png");
    starsSprite->setID("stars-sprite");
    if (is_platformer) starsSprite->setScale(0.8f);
    starsSprite->setPosition({(width - 100.f)/3.f*2.f - 30.f, HEIGHT / 3.f});
    addChild(starsSprite);

    starsLabel = CCLabelBMFont::create(floatToString(level.averageRating, 2).c_str(), "bigFont.fnt");
    starsLabel->setID("stars-label");
    starsLabel->setAnchorPoint({0.f, 0.5f});
    starsLabel->setPosition({(width - 100.f)/3.f*2.f - 20.f, HEIGHT / 3.f});
    starsLabel->limitLabelWidth(100.f, 0.5f, .1f);
    addChild(starsLabel);

    featureSprite = CCSprite::createWithSpriteFrameName("GJ_sTrendingIcon_001.png");
    featureSprite->setID("feature-sprite");
    featureSprite->setPosition({(width - 100.f)/3.f*3.f - 30.f, HEIGHT / 3.f});
    addChild(featureSprite);

    featureLabel = CCLabelBMFont::create(floatToString(level.averageFeature, 2).c_str(), "bigFont.fnt");
    featureLabel->setID("feature-label");
    featureLabel->setAnchorPoint({0.f, 0.5f});
    featureLabel->setPosition({(width - 100.f)/3.f*3.f - 20.f, HEIGHT / 3.f});
    featureLabel->limitLabelWidth(100.f, 0.5f, .1f);
    addChild(featureLabel);

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