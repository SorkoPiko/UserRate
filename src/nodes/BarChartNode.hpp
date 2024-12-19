#include <utility>

#ifndef BARCHARTNODE_HPP
#define BARCHARTNODE_HPP

using namespace geode::prelude;

// this is all 100% taken from GDDL Integration, MASSIVE thanks to B1rtek for this
// https://github.com/B1rtek/Geode-GDDLIntegration/blob/8cecefc2199b7ade8a9266328893db075710f3b2/src/nodes/BarChartNode.h

struct BarChartData {
    const _ccColor4F color;
    const std::string label;
    const int value;

    BarChartData(const _ccColor4F &color, std::string label, const int value) : color(color), label(std::move(label)), value(value) {}
};

struct BarChartBar {
    const _ccColor4F color;
    const float height;

    BarChartBar(const _ccColor4F &color, const float height) : color(color), height(height) {}
};

class BarChartNode final : public CCNode {
    std::vector<BarChartBar> processedData{};
    float barHeight = 20.0f;
    float labelWidth = 30.0f;

    bool init(std::vector<BarChartData> data, const CCSize &size, float labelWidth, float barHeight);

    BarChartBar calculateBar(const BarChartData &barData, int index, int sum, float chartSize, int maxValue);

public:
    static BarChartNode *create(const std::vector<BarChartData> &data, const CCSize &size, float labelWidth, float barHeight);

    void draw() override;
};


#endif
