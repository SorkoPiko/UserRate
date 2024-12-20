#ifndef RANGEOPTION_HPP
#define RANGEOPTION_HPP

#include "Option.hpp"

class RangeOption final : public Option {
    std::optional<int> minValue;
    std::optional<int> maxValue;

    CCMenu* menu = nullptr;
    TextInput* lowTextField = nullptr;
    TextInput* highTextField = nullptr;

    void onLowLeft(CCObject*);
    void onLowRight(CCObject*);
    void onHighLeft(CCObject*);
    void onHighRight(CCObject*);

    bool init(const std::string& label, std::optional<int> maxChars, std::optional<int> minValue, std::optional<int> maxValue, std::optional<int> lowValue, std::optional<int> highValue);

public:
    static RangeOption* create(const std::string& label, std::optional<int> maxChars, std::optional<int> minValue, std::optional<int> maxValue, std::optional<int> lowValue, std::optional<int> highValue);

    void updateValues();

    std::optional<int> lowValue;
    std::optional<int> highValue;
};

#endif
