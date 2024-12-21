#ifndef VIEWSENTLEVELPOPUP_HPP
#define VIEWSENTLEVELPOPUP_HPP

#include "AdminPage.hpp"
#include "../nodes/BarChartNode.hpp"
#include "../types/SentLevel.hpp"

using namespace geode::prelude;

class ViewSentLevelPopup final : public Popup<const SentLevel&, AdminPage*>{
    // thanks B1rtek
    // (https://github.com/B1rtek/Geode-GDDLIntegration/blob/8cecefc2199b7ade8a9266328893db075710f3b2/src/objects/RatingsSpread.h#L14)
    static constexpr int ratingColours[] = {
        0xe67c73,
        0xea8b71,
        0xf2a96d,
        0xf6b86b,
        0xfac769,
        0xffd666,
        0xd5d06f,
        0xabc978,
        0x81c281,
        0x57bb8a
    };

    static constexpr int featureColours[] = {
        0xe67c73,
        0xf2a96d,
        0xfac769,
        0xabc978,
        0x57bb8a
    };

    static constexpr const char* featureNames[] = {
        "Rate",
        "Feature",
        "Epic",
        "Legendary",
        "Mythic"
    };

    AdminPage* adminPage = nullptr;
    SentLevel level;

    CCLabelBMFont* title = nullptr;
    BarChartNode* ratingChart = nullptr;
    BarChartNode* featureChart = nullptr;
    CCSprite* separator = nullptr;
    CCLabelBMFont* ratingLabel = nullptr;
    CCLabelBMFont* featureLabel = nullptr;
    CCMenuItemSpriteExtra* playButton = nullptr;
    CCMenuItemSpriteExtra* clearButton = nullptr;

    bool setup(const SentLevel& level, AdminPage* adminPage) override;

    static ccColor4F hexColorTo4F(int hexColor);
    static std::string floatToString(float number, int precision);

    [[nodiscard]] std::vector<BarChartData> getRatingData() const;
    [[nodiscard]] std::vector<BarChartData> getFeatureData() const;

    void onPlay(CCObject*);
    void onClear(CCObject*);

public:
    static ViewSentLevelPopup* create(const SentLevel& level, AdminPage* adminPage);
};

#endif
