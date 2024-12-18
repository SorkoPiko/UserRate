#include "SentPopup.hpp"

SentPopup* SentPopup::create() {
    const auto ret = new SentPopup();
    if (ret->initAnchored(368, 400, "square01_001.png", {0.f, 0.f, 94.f, 94.f})) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

// this is mainly stolen from SecretWays (https://github.com/SorkoPiko/SecretWay-Mod/blob/dcadee7b01bac8685fcd4dc5938714db31d9bf0b/src/layers/SWLayer.cpp#L19)
// and Death Tracker (https://github.com/eloh-mrow/death-tracker/blob/f72f9dc9af1d5fc0d4c5d514361a2fa4e53c39cc/src/nodes/layers/DTLayer.cpp#L22)

bool SentPopup::setup() {
    const auto winSize = CCDirector::sharedDirector()->getWinSize();

    setZOrder(106);

    this->setTitle("Sent Levels", "goldFont.fnt", 1.5f);

    m_bg = CCScale9Sprite::create("GJ_square05.png", {0,0, 80, 80});
    m_bg->setContentSize({m_size.width / 1.6f, m_size.height / 1.25f});
    m_bg->setPosition({winSize.width / 2 + 60.f, winSize.height / 2 - 30.f});
    m_mainLayer->addChild(m_bg);

    m_scrollLayer = ScrollLayer::create({m_size.width / 1.6f - 20.f, m_size.height / 1.25f - 20.f});
    m_scrollLayer->setPosition(CCSize{winSize.width / 2 + 70.f, winSize.height / 2 - 20.f} - m_bg->getContentSize() / 2);
    m_scrollLayer->setZOrder(11);
    m_mainLayer->addChild(m_scrollLayer);

    m_scrollBar = Scrollbar::create(m_scrollLayer);
    m_scrollBar->setScale(1);
    m_scrollBar->setZOrder(11);
    m_scrollBar->setPosition({499, 130});
    m_mainLayer->addChild(m_scrollBar);

    loadingCircle = LoadingCircle::create();
    loadingCircle->setParentLayer(m_mainLayer);
    loadingCircle->show();
    loadingCircle->setZOrder(1000);
    loadingCircle->setVisible(true);
    loadingCircle->setPosition(-loadingCircle->getContentSize() / 2 + m_size / 2);

    return true;
}
