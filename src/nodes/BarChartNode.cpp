#include "BarChartNode.hpp"

std::string floatToString(const float number, const int precision) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << number;
    return stream.str();
}

bool BarChartNode::init(std::vector<BarChartData> data, const CCSize& size, const float labelWidth, const float barHeight) {
    if (!CCNode::init()) return false;

    setContentSize(size);
    this->labelWidth = labelWidth;
    this->barHeight = barHeight;

    int sum = 0, maxValue = 0;
    std::ranges::for_each(data, [&sum, &maxValue](const BarChartData& barData) {
        sum += barData.value;
        maxValue = std::max(barData.value, maxValue);
    });
    for (int i = 0; i < data.size(); i++) {
        processedData.push_back(calculateBar(data[i], i, sum, size.width, maxValue));
    }

    return true;
}

BarChartBar BarChartNode::calculateBar(const BarChartData &barData, const int index, const int sum, const float chartSize, const int maxValue) {
    const float percent = static_cast<float>(barData.value) / static_cast<float>(sum);
    float height = (chartSize - labelWidth) * (static_cast<float>(barData.value) / static_cast<float>(sum)) * (static_cast<float>(sum) / static_cast<float>(maxValue));
    const std::string percentage = std::to_string(barData.value) + " (" + floatToString(percent * 100, 2) + "%)";
    const auto labelLabel = CCLabelBMFont::create(barData.label.c_str(), "chatFont.fnt");
    labelLabel->setScale(0.6f * labelLabel->getContentSize().width > labelWidth ? labelWidth / labelLabel->getContentSize().width : 0.6f);
    labelLabel->setAnchorPoint({0.0f, 0.5f});
    const float labelPositionAdjustement = labelLabel->getContentWidth() * (CCDirector::get()->getLoadedTextureQuality() == kTextureQualityHigh ? 0.0f : 0.05f);
    labelLabel->setPosition({0.0f - labelPositionAdjustement, getContentHeight() - barHeight * index - barHeight / 2});
    if (labelLabel->getScaledContentHeight() > barHeight) labelLabel->setScale(labelLabel->getScale() * (barHeight / labelLabel->getScaledContentHeight()));
    addChild(labelLabel);
    const auto percentageLabel = CCLabelBMFont::create(percentage.c_str(), "chatFont.fnt");
    labelLabel->setScale(0.6f * percentageLabel->getContentSize().width > labelWidth * 1.5f ? labelWidth * 1.5f / percentageLabel->getContentSize().width : 0.6f);
    percentageLabel->setAnchorPoint({0.0f, 0.5f});
    percentageLabel->setPosition({labelWidth + height + 2.0f, getContentHeight() - barHeight * index - barHeight / 2});
    if (percentageLabel->getScaledContentHeight() > barHeight) percentageLabel->setScale(percentageLabel->getScale() * (barHeight / percentageLabel->getScaledContentHeight()));
    addChild(percentageLabel);
    return {barData.color, height};
}


BarChartNode* BarChartNode::create(const std::vector<BarChartData>& data, const CCSize& size, const float labelWidth, const float barHeight) {
    if (const auto newNode = new BarChartNode(); newNode->init(data, size, labelWidth, barHeight)) {
        newNode->autorelease();
        return newNode;
    } else {
        delete newNode;
        return nullptr;
    }
}

void BarChartNode::draw() {
    CCNode::draw();
    const float padding = Mod::get()->getSettingValue<bool>("thin-chart-bars") ? barHeight * 0.05f : 0.0f;
    for (int i = 0; i < processedData.size(); i++) {
        ccDrawSolidRect(
            {labelWidth, getContentHeight() - barHeight * i - padding},
            {labelWidth + processedData[i].height,
            getContentHeight() - barHeight * (i + 1) + padding}, processedData[i].color
        );
    }
}