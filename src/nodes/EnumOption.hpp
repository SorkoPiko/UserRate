#ifndef ENUMOPTION_HPP
#define ENUMOPTION_HPP

#include "Option.hpp"

class EnumOption final : public Option {
    std::vector<std::string> options;
    int index = 0;
    std::function<void(int)> callback;

    CCMenu* menu = nullptr;
    CCLabelBMFont* label = nullptr;
    CCScale9Sprite* bg = nullptr;

    void onLeft(CCObject*);
    void onRight(CCObject*);

    bool init(const std::string& label, std::vector<std::string> options, std::optional<int> defaultValue, const std::function<void(int)>& callback);

public:
    static EnumOption* create(const std::string& label, const std::vector<std::string>& options, std::optional<int> defaultValue, const std::function<void(int)>& callback);

    void updateValues() const;
};

#endif
