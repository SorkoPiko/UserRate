#include "EnumOption.hpp"

#include <utility>

EnumOption* EnumOption::create(const std::string& label, const std::vector<std::string>& options, const std::optional<int> defaultValue, const std::function<void(int)>& callback) {
    const auto ret = new EnumOption();
    if (ret->init(label, options, defaultValue, callback)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool EnumOption::init(const std::string& label, std::vector<std::string> options, const std::optional<int> defaultValue, const std::function<void(int)>& callback) {
    if (!Option::init()) return false;

    this->options = std::move(options);
    index = defaultValue.value_or(0);
    this->callback = callback;

    log::info("Index: {}, Options: {}", index, this->options);

    if (index < 0 || index >= this->options.size()) {
        log::warn("Index {} out of bounds for EnumOption with options {}", index, this->options);
        index = 0;
    }

    menu = CCMenu::create();
    menu->setID("button-menu");
    menu->setContentSize(getContentSize());
    addChildAtPosition(menu, Anchor::BottomLeft);

    createLabel(menu, "bigFont.fnt", label, 110.0f, {0.f, 15.f});
    bg = createEnumLabel(menu, this->label, "bigFont.fnt", this->options.at(index), 110.0f, {0.f, -10.f}, {110.0f, 25.0f});
    createStepperButtons(bg, {13.0f, 19.0f}, this, menu_selector(EnumOption::onLeft), menu_selector(EnumOption::onRight));

    return true;
}

void EnumOption::onLeft(CCObject*) {
    if (index == 0) index = options.size() - 1;
    else index--;
    label->setString(options[index].c_str());
    label->setScale(0.6f * label->getContentSize().width > 110.f ? 110.f / label->getContentSize().width : 0.6f);
}


void EnumOption::onRight(CCObject*) {
    if (index == options.size() - 1) index = 0;
    else index++;
    label->setString(options[index].c_str());
    label->setScale(0.6f * label->getContentSize().width > 110.f ? 110.f / label->getContentSize().width : 0.6f);
}

void EnumOption::updateValues() const {
    callback(index);
}
