#ifndef ADMINPAGE_HPP
#define ADMINPAGE_HPP

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class AdminPage final : public CCLayer {
    CCNode* mainLayer = nullptr;
    CCSprite* background = nullptr;
    CCMenu* closeMenu = nullptr;
    CCMenu* buttonMenu = nullptr;
    CustomListView* levelList = nullptr;

    bool init() override;

    void onBack(CCObject*);
    void keyBackClicked() override;

    void createBorders() const;

public:
    static AdminPage* create();
};

#endif
