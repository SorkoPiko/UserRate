#ifndef SENTLEVELCELL_HPP
#define SENTLEVELCELL_HPP

#include "AdminPage.hpp"
#include "../nodes/DifficultyNode.hpp"
#include "../types/SentLevel.hpp"

using namespace geode::prelude;

class SentLevelCell final : public CCLayer {
    AdminPage* adminPage = nullptr;
    SentLevel level;

    CCLabelBMFont* nameLabel = nullptr;
    DifficultyNode* diffNode = nullptr;
    CCSprite* sendsSprite = nullptr;
    CCLabelBMFont* sendsLabel = nullptr;
    CCSprite* starsSprite = nullptr;
    CCLabelBMFont* starsLabel = nullptr;
    CCSprite* featureSprite = nullptr;
    CCLabelBMFont* featureLabel = nullptr;
    CCMenu* buttonMenu = nullptr;
    CCMenuItemSpriteExtra* viewButton = nullptr;

    bool init(const SentLevel& level, float width, AdminPage* adminPage);

    static std::string floatToString(float number, int precision);

    void onView(CCObject*);

public:
    static SentLevelCell* create(const SentLevel& level, float width, AdminPage* adminPage);

    static constexpr int HEIGHT = 75;
};

#endif
