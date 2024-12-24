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
    CCMenu* refreshMenu = nullptr;
    CCMenu* pageMenu = nullptr;
    CCMenuItemSpriteExtra* refreshButton = nullptr;
    CCMenuItemSpriteExtra* prevPageButton = nullptr;
    CCMenuItemSpriteExtra* nextPageButton = nullptr;
    LoadingCircle* loadingCircle = nullptr;

    SentLevelFilters filters{};
    bool ready = false;

    bool init() override;

    void openFilters(CCObject*);
    void refresh(CCObject*);
    void onPrevPage(CCObject*);
    void onNextPage(CCObject*);
    void onBack(CCObject*);
    void keyBackClicked() override;

    void loadUI();
    void finishLoadUI() const;

    void createBorders() const;

public:
    static AdminPage* create();

    void loadLevelPage();
};

#endif
