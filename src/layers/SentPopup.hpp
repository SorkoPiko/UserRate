#ifndef SENTPOPUP_HPP
#define SENTPOPUP_HPP

using namespace geode::prelude;

class SentPopup final : public Popup<> {
    CCScale9Sprite* m_bg = nullptr;
    ScrollLayer* m_scrollLayer = nullptr;
    Scrollbar* m_scrollBar = nullptr;
    LoadingCircle* loadingCircle = nullptr;

    bool setup() override;

public:
    static SentPopup* create();
};

#endif
