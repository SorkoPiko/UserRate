#include "ViewSentLevelPopup.hpp"

#include <cmath>

#include "../managers/API.hpp"
#include "../managers/Global.hpp"

ViewSentLevelPopup* ViewSentLevelPopup::create(const SentLevel& level, AdminPage* adminPage) {
    const auto ret = new ViewSentLevelPopup();
    if (ret->initAnchored(440.f, 280.f, level, adminPage, "GJ_square05.png", {0.f, 0.f, 80.f, 80.f})) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool ViewSentLevelPopup::setup(const SentLevel& level, AdminPage* adminPage) {
    this->level = level;
    this->adminPage = adminPage;

    ready = true;

    const auto width = m_mainLayer->getContentWidth();

    title = CCLabelBMFont::create("Level Info", "bigFont.fnt");
    title->setID("title");
    title->setPosition({width/2.f, m_mainLayer->getContentHeight() - 25.f});
    m_mainLayer->addChild(title, 1);

    constexpr float ratingBarHeight = 15.f;
    const auto ratingData = getRatingData();
    ratingChart = BarChartNode::create(ratingData, {150.f, 10.f * ratingBarHeight}, 30.f, ratingBarHeight);
    ratingChart->setID("rating-chart");
    ratingChart->setAnchorPoint({0.f, 1.f});
    ratingChart->setPosition({10.f, 180.f});
    m_mainLayer->addChild(ratingChart);

    constexpr float featureBarHeight = 30.f;
    const auto featureData = getFeatureData();
    featureChart = BarChartNode::create(featureData, {150.f, 5.f * featureBarHeight}, 50.f, featureBarHeight);
    featureChart->setID("feature-chart");
    featureChart->setAnchorPoint({0.f, 1.f});
    featureChart->setPosition({10.f+width/2.f, 180.f});
    m_mainLayer->addChild(featureChart);

    separator = CCSprite::createWithSpriteFrameName("floorLine_001.png");
    separator->setID("seperator");
    separator->setRotation(90.0f);
    separator->setPosition({width/2.f, 105.f});
    separator->setScale(0.4f);
    m_mainLayer->addChild(separator);

    const auto [name, isPlatformer] = Global::get()->getLevel(level.id);

    ratingLabel = CCLabelBMFont::create(
        fmt::format("Average: {} {} out of {} sends", floatToString(level.averageRating, 2),
        isPlatformer ? "moons" : "stars", level.totalSends).c_str(),
        "chatFont.fnt"
    );
    ratingLabel->setID("rating-label");
    ratingLabel->setScale(0.7f);
    ratingLabel->setAnchorPoint({0.f, 0.f});
    ratingLabel->setPosition({10.f, 195.f});
    m_mainLayer->addChild(ratingLabel);

    featureLabel = CCLabelBMFont::create(
        fmt::format("Average: {} ({}) out of {} sends", floatToString(level.averageFeature, 2), featureNames[static_cast<int>(std::round(level.averageFeature))],
        level.totalSends).c_str(),
        "chatFont.fnt"
    );
    featureLabel->setID("feature-label");
    featureLabel->setScale(0.7f);
    featureLabel->setAnchorPoint({0.f, 0.f});
    featureLabel->setPosition({10.f+width/2.f, 195.f});
    m_mainLayer->addChild(featureLabel);

    playButton = CCMenuItemSpriteExtra::create(
        [] {
            const auto sprite = CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
            sprite->setScale(0.5f);
            return sprite;
        }(),
        this,
        menu_selector(ViewSentLevelPopup::onPlay)
    );
    playButton->setID("play-button");
    playButton->setPosition(m_mainLayer->getContentSize());
    m_buttonMenu->addChild(playButton);

    if (Global::get()->isAdmin()) {
        clearButton = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png"),
            this,
            menu_selector(ViewSentLevelPopup::onClear)
        );
        clearButton->setID("clear-button");
        clearButton->setPosition({m_mainLayer->getContentWidth(), m_mainLayer->getContentHeight() - 40.f});
        m_buttonMenu->addChild(clearButton);
    }

    return true;
}

void ViewSentLevelPopup::onClose(CCObject* sender) {
    ready = false;
    Popup::onClose(sender);
}

ccColor4F ViewSentLevelPopup::hexColorTo4F(const int hexColor) {
    const float r = (hexColor >> 16 & 0xFF) / 255.0;
    const float g = (hexColor >> 8 & 0xFF) / 255.0;
    const float b = (hexColor & 0xFF) / 255.0;
    return ccc4f(r, g, b, 1.0f);
}

std::string ViewSentLevelPopup::floatToString(const float number, const int precision) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << number;
    return stream.str();
}

std::vector<BarChartData> ViewSentLevelPopup::getRatingData() const {
    std::vector<BarChartData> data;
    data.reserve(level.ratingSends.size());
    for (const auto& [key, val] : level.ratingSends) {
        data.emplace_back(
            hexColorTo4F(ratingColours[key - 1]),
            std::to_string(key),
            val
        );
    }

    return data;
}

std::vector<BarChartData> ViewSentLevelPopup::getFeatureData() const {
    std::vector<BarChartData> data;
    data.reserve(level.featureSends.size());
    for (const auto& [key, val] : level.featureSends) {
        log::info("key: {}, val: {}", featureNames[key], val);
        data.emplace_back(
            hexColorTo4F(featureColours[key]),
            featureNames[key],
            val
        );
    }

    return data;
}

void ViewSentLevelPopup::onPlay(CCObject*) {
    const auto scene = CCScene::create();
    const auto browseLayer = LevelBrowserLayer::create(GJSearchObject::create(SearchType::Type19, std::to_string(level.id)));
    scene->addChild(browseLayer);
    CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
}

void ViewSentLevelPopup::onClear(CCObject*) {
    createQuickPopup(
        "Clear Level Sends",
        "Are you sure you want to clear this level's sends?",
        "Cancel", "Clear",
        [this](auto, const bool btn2) {
            if (btn2) API::clearLevelSends(level.id, [this](const bool success) {
                if (success) {
                    FLAlertLayer::create(
                        "Clear Level Sends",
                        "Success! The level's sends have been cleared.",
                        "OK"
                    )->show();

                    Global::get()->clearLevelPages();

                    if (!ready) return;

                    adminPage->loadLevelPage();
                    onClose(nullptr);
                }
            });
        }
    )->m_button1->updateBGImage("GJ_button_06.png");

}