#include "LoadLayer.hpp"

LoadLayer* LoadLayer::create() {
    const auto ret = new LoadLayer();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

LoadLayer* LoadLayer::createHidden() {
    const auto ret = new LoadLayer();
    if (ret->initHidden()) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool LoadLayer::init() {
    if (!initWithColor({0, 0, 0, 75})) return false;
    m_mainLayer = CCLayer::create();
    m_buttonMenu = CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu);
    addChild(m_mainLayer);
    registerWithTouchDispatcher();
    setTouchEnabled(true);
    setKeyboardEnabled(true);

    m_loadingCircle = LoadingCircle::create();
    m_loadingCircle->setParentLayer(m_mainLayer);
    m_loadingCircle->show();
    return true;
}

bool LoadLayer::initHidden() {
    if (!initWithColor({0, 0, 0, 0})) return false;
    m_mainLayer = CCLayer::create();
    m_buttonMenu = CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu);
    addChild(m_mainLayer);
    registerWithTouchDispatcher();
    setTouchEnabled(true);
    setKeyboardEnabled(true);
    return true;
}


void LoadLayer::finished() {
    removeFromParentAndCleanup(true);
}