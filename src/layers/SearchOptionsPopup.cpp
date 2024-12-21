#include "SearchOptionsPopup.hpp"

#include "../managers/Global.hpp"

SearchOptionsPopup* SearchOptionsPopup::create(SentLevelFilters* filters, AdminPage* adminPage) {
    const auto ret = new SearchOptionsPopup();
    if (ret->initAnchored(300.f, 160.f, filters, adminPage, "GJ_square01.png", {0.f, 0.f, 80.f, 80.f})) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool SearchOptionsPopup::setup(SentLevelFilters* filters, AdminPage* adminPage) {
    this->filters = filters;
    this->adminPage = adminPage;
    originalFilters = *filters;

    const auto width = m_mainLayer->getContentWidth();
    const auto height = m_mainLayer->getContentHeight() - 30;

    m_buttonMenu = CCMenu::create();
    m_buttonMenu->setID("button-menu");
    m_buttonMenu->setContentSize(m_mainLayer->getContentSize());
    m_mainLayer->addChildAtPosition(m_buttonMenu, Anchor::BottomLeft);

    title = CCLabelBMFont::create("Options", "bigFont.fnt");
    title->setID("title");
    title->setPosition({width/2.f, height + 10.f});
    m_mainLayer->addChild(title, 1);

    sendsOption = RangeOption::create(
        "Sends",
        std::nullopt,
        0,
        std::nullopt,
        filters->minSends,
        filters->maxSends
    );
    sendsOption->setID("sends-option");
    sendsOption->setPosition({width/4, height/4*3});
    m_mainLayer->addChild(sendsOption);

    starsOption = RangeOption::create(
        "Stars",
        2,
        1,
        10,
        filters->minAvgStars,
        filters->maxAvgStars
    );
    starsOption->setID("stars-option");
    starsOption->setPosition({width/4*3, height/4*3});
    m_mainLayer->addChild(starsOption);

    featureOption = RangeOption::create(
        "Feature",
        1,
        0,
        4,
        filters->minAvgFeature,
        filters->maxAvgFeature
    );
    featureOption->setID("feature-option");
    featureOption->setPosition({width/4, height/4});
    m_mainLayer->addChild(featureOption);

    typeOption = EnumOption::create(
        "Type",
        {SentLevelFilters::sortToString(TOP), SentLevelFilters::sortToString(RECENT), SentLevelFilters::sortToString(OLDEST)},
        filters->sort,
        [filters](int index) {
            filters->sort = static_cast<SentLevelSearchType>(index);
        }
    );
    typeOption->setID("type-option");
    typeOption->setPosition({width/4*3, height/4});
    m_mainLayer->addChild(typeOption);

    return true;
}

void SearchOptionsPopup::onClose(CCObject *sender) {\
    sendsOption->updateValues();
    filters->minSends = sendsOption->lowValue.value_or(0);
    filters->maxSends = sendsOption->highValue;

    starsOption->updateValues();
    filters->minAvgStars = starsOption->lowValue.value_or(1);
    filters->maxAvgStars = starsOption->highValue.value_or(10);

    featureOption->updateValues();
    filters->minAvgFeature = featureOption->lowValue.value_or(0);
    filters->maxAvgFeature = featureOption->highValue.value_or(4);

    typeOption->updateValues();

    if (*filters != originalFilters) {
        Global::get()->clearLevelPages();
        filters->page = 0;
    }

    adminPage->loadLevelPage();
    Popup::onClose(sender);
}