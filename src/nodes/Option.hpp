#ifndef OPTION_HPP
#define OPTION_HPP

using namespace geode::prelude;

class Option : public CCNode {
protected:
    static void createStepperButtons(CCNode* object, const CCPoint& size, CCObject* callbackObject, SEL_MenuHandler leftCallback, SEL_MenuHandler rightCallback, int zOrder = 1);
    static void createTextInput(CCNode* parent, TextInput*& textfield, const std::string& font, const std::string& placeholder, std::optional<int> maxCharacters, const CCPoint& bgSize, const CCPoint& position, int zOrder = 1);
    static void createLabel(CCNode* parent, const std::string& font, const std::string& text, float maxWidth, const CCPoint& position, int zOrder = 1);
    static CCScale9Sprite* createEnumLabel(CCLayer* parent, CCLabelBMFont*& label, const std::string& font, const std::string& placeholder, float maxWidth, const CCPoint& position, const CCPoint& bgSize, int zOrder = 1);
    static void setNumber(std::optional<int> value, TextInput*& textField, std::optional<int> defaultValue);
    static std::optional<int> getNumber(TextInput*& textfield, std::optional<int> defaultValue);
};

#endif
