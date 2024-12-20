#include "Option.hpp"

// thanks B1rtek for these amazing but amazingly messy helper functions
// https://github.com/B1rtek/Geode-GDDLIntegration/blob/8cecefc2199b7ade8a9266328893db075710f3b2/src/Utils.h
void Option::createStepperButtons(CCNode* object, const CCPoint& size, CCObject* callbackObject, const SEL_MenuHandler leftCallback, const SEL_MenuHandler rightCallback, const int zOrder) {
    const CCPoint positionLeft = object->getPosition() - CCPoint(object->getContentSize().width * object->getScale() / 2 + size.x / 2 + 0.5f, -1.0f);
    const auto leftButtonSprite = CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
    leftButtonSprite->setScale(0.8f);

    const auto leftButton = CCMenuItemSpriteExtra::create(leftButtonSprite, callbackObject, leftCallback);
    leftButton->setPosition(positionLeft);
    leftButton->setContentSize(size);
    object->getParent()->addChild(leftButton, zOrder);

    const CCPoint positionRight = positionLeft + CCPoint(object->getContentSize().width * object->getScale() + leftButton->getContentSize().width + 2.5f, 0.0f);
    const auto rightButtonSprite = CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
    rightButtonSprite->setScale(0.8f);

    const auto rightButton = CCMenuItemSpriteExtra::create(rightButtonSprite, callbackObject, rightCallback);
    rightButton->setPosition(positionRight);
    rightButton->setContentSize(size);
    object->getParent()->addChild(rightButton, zOrder);
}

void Option::createTextInput(CCNode* parent, TextInput*& textfield, const std::string& font, const std::string& placeholder, const std::optional<int> maxCharacters, const CCPoint& bgSize, const CCPoint& position, const int zOrder) {
    const auto bg = CCScale9Sprite::create("square02_small.png");
    bg->setContentSize(bgSize);
    bg->setScale(0.5f);
    bg->setContentSize(bg->getContentSize() * 2.f);
    bg->setPosition(position);
    bg->setOpacity(100);
    parent->addChild(bg, zOrder);

    textfield = TextInput::create(bgSize.x, placeholder, font);
    textfield->setPosition(position);
    textfield->setMaxCharCount(maxCharacters.value_or(0));
    textfield->setCommonFilter(CommonFilter::Int);
    textfield->hideBG();
    parent->addChild(textfield, zOrder + 1);
}

void Option::createLabel(CCNode* parent, const std::string& font, const std::string& text, const float maxWidth, const CCPoint& position, const int zOrder) {
    const auto label = CCLabelBMFont::create(text.c_str(), font.c_str());
    label->setPosition(position);
    label->setScale(0.6f * label->getContentSize().width > maxWidth ? maxWidth / label->getContentSize().width : 0.6f);
    parent->addChild(label, zOrder);
}

void Option::setNumber(std::optional<int> value, TextInput*& textField, std::optional<int> defaultValue) {
    if (value.has_value()) textField->setString(fmt::format("{}", value.value()));
    else if (defaultValue.has_value()) textField->setString(fmt::format("{}", defaultValue.value()));
    else textField->setString("");
}

std::optional<int> Option::getNumber(TextInput*& textfield, const std::optional<int> defaultValue) {
    if (textfield->getString().empty()) return defaultValue;
    if (auto returnValueResult = numFromString<int>(textfield->getString()); returnValueResult.isOk()) return returnValueResult.unwrap();
    return defaultValue;
}