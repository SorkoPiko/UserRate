#ifndef ADMINPAGE_HPP
#define ADMINPAGE_HPP

#include <Geode/Geode.hpp>

#include "../types/SentLevelFilters.hpp"

using namespace geode::prelude;

class AdminPage final : public CCLayer {
    CCNode* mainLayer = nullptr;
    CCSprite* background = nullptr;
    CCMenu* closeMenu = nullptr;
    CCMenu* buttonMenu = nullptr;
    CCNode* listLayer = nullptr;
    CustomListView* levelList = nullptr;
    CCLayerColor* listBG = nullptr;
    CCLabelBMFont* title = nullptr;
    CCMenu* optionsMenu = nullptr;

    SentLevelFilters filters{};

    bool init() override;

    void openFilters(CCObject*);

    void loadLevelPage();

    void onBack(CCObject*);
    void keyBackClicked() override;

    void createBorders() const;

public:
    static AdminPage* create();
};

#endif
