#include "RangeOption.hpp"

RangeOption* RangeOption::create(const std::string& label, const std::optional<int> maxChars, const std::optional<int> minValue, const std::optional<int> maxValue, const std::optional<int> lowValue, const std::optional<int> highValue) {
    const auto ret = new RangeOption();
    if (ret->init(label, maxChars, minValue, maxValue, lowValue, highValue)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool RangeOption::init(const std::string& label, const std::optional<int> maxChars, const std::optional<int> minValue, const std::optional<int> maxValue, const std::optional<int> lowValue, const std::optional<int> highValue) {
    if (!Option::init()) return false;

    this->minValue = minValue;
    this->maxValue = maxValue;
    this->lowValue = lowValue;
    this->highValue = highValue;

    menu = CCMenu::create();
    menu->setID("button-menu");
    menu->setContentSize(getContentSize());
    addChildAtPosition(menu, Anchor::BottomLeft);

    createLabel(menu, "bigFont.fnt", label, 110.0f, {0.f, 15.f});
    createLabel(menu, "chatFont.fnt", "to", 30.0f, {0.f, -10.f});

    createTextInput(menu, lowTextField, "bigFont.fnt", "", maxChars, {35.0f, 25.0f}, {-37.5f, -10.f});
    lowTextField->setCallback([this](const std::string& value) {
        if (value.empty()) return;
        if (const auto result = numFromString<int>(value); result.isOk()) {
            if (const auto newValue = result.unwrap(); this->minValue.has_value() && newValue < this->minValue.value()) setNumber(this->minValue, lowTextField, this->minValue);
            else if (this->maxValue.has_value() && newValue > this->maxValue.value()) setNumber(this->maxValue, lowTextField, this->minValue);
            else if (const auto readHighValue = getNumber(highTextField, this->maxValue); newValue > readHighValue) {
                int newSetValue;
                if (newValue > this->maxValue.value_or(newValue)) newSetValue = this->maxValue.value();
                else newSetValue = newValue;
                setNumber(newSetValue, lowTextField, this->minValue);
                setNumber(newSetValue, highTextField, this->maxValue);
            }
        }
    });
    createStepperButtons(lowTextField, {13.0f, 19.0f}, this, menu_selector(RangeOption::onLowLeft), menu_selector(RangeOption::onLowRight));
    setNumber(lowValue, lowTextField, minValue);

    createTextInput(menu, highTextField, "bigFont.fnt", "", maxChars, {35.0f, 25.0f}, {37.5f, -10.f});
    highTextField->setCallback([this](const std::string& value) {
        if (value.empty()) return;
        if (const auto result = numFromString<int>(value); result.isOk()) {
            if (const auto newValue = result.unwrap(); this->minValue.has_value() && newValue < this->minValue.value()) setNumber(this->minValue, highTextField, this->maxValue);
            else if (this->maxValue.has_value() && newValue > this->maxValue.value()) setNumber(this->maxValue, highTextField, this->maxValue);
            else if (const auto readLowValue = getNumber(lowTextField, this->minValue); newValue < readLowValue) {
                int newSetValue;
                if (newValue < this->minValue.value_or(newValue)) newSetValue = this->minValue.value();
                else newSetValue = newValue;
                setNumber(newSetValue, lowTextField, this->minValue);
                setNumber(newSetValue, highTextField, this->maxValue);
            }
        }
    });
    createStepperButtons(highTextField, {13.0f, 19.0f}, this, menu_selector(RangeOption::onHighLeft), menu_selector(RangeOption::onHighRight));
    setNumber(highValue, highTextField, maxValue);

    return true;
}

void RangeOption::onLowLeft(CCObject*) {
    int newValue = getNumber(lowTextField, minValue).value_or(0) - 1;
    if (minValue.has_value() && newValue < minValue.value()) newValue = minValue.value();
    setNumber(newValue, lowTextField, minValue);
}

void RangeOption::onLowRight(CCObject*) {
    int newValue = getNumber(lowTextField, minValue).value_or(0) + 1;
    if (maxValue.has_value() && newValue > maxValue.value()) newValue = maxValue.value();
    if (const auto highValue = getNumber(highTextField, maxValue).value_or(0); newValue > highValue) {
        onHighRight(nullptr);
        newValue = getNumber(highTextField, maxValue).value_or(0);
    }
    setNumber(newValue, lowTextField, minValue);
}

void RangeOption::onHighLeft(CCObject*) {
    int newValue = getNumber(highTextField, maxValue).value_or(0) - 1;
    if (minValue.has_value() && newValue < minValue.value()) newValue = minValue.value();
    if (const int lowValue = getNumber(lowTextField, minValue).value_or(0); newValue < lowValue) {
        onLowLeft(nullptr);
        newValue = getNumber(lowTextField, minValue).value_or(0);
    }
    setNumber(newValue, highTextField, maxValue);
}

void RangeOption::onHighRight(CCObject*) {
    int newValue = getNumber(highTextField, maxValue).value_or(0) + 1;
    if (maxValue.has_value() && newValue > maxValue.value()) newValue = maxValue.value();
    setNumber(newValue, highTextField, maxValue);
}

void RangeOption::updateValues() {
    lowValue = getNumber(lowTextField, std::nullopt);
    highValue = getNumber(highTextField, std::nullopt);
}