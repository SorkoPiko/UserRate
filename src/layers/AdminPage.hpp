#ifndef ADMINPAGE_HPP
#define ADMINPAGE_HPP

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class AdminPage : public CCLayer {

    CCNode* mainLayer = nullptr;
    CCSprite* background = nullptr;
    CCMenuItemSpriteExtra* closeBtn = nullptr;

    bool init();

public:
    static AdminPage* create();
};

#endif
