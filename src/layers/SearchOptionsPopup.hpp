#ifndef SEARCHOPTIONSPOPUP_HPP
#define SEARCHOPTIONSPOPUP_HPP
#include "AdminPage.hpp"
#include "../nodes/EnumOption.hpp"
#include "../nodes/RangeOption.hpp"
#include "../types/SentLevelFilters.hpp"

using namespace geode::prelude;

class SearchOptionsPopup final : public Popup<SentLevelFilters*, AdminPage*> {
    AdminPage* adminPage = nullptr;
    SentLevelFilters* filters = nullptr;
    SentLevelFilters originalFilters;

    CCLabelBMFont* title = nullptr;
    RangeOption* sendsOption = nullptr;
    RangeOption* starsOption = nullptr;
    RangeOption* featureOption = nullptr;
    EnumOption* typeOption = nullptr;

    bool setup(SentLevelFilters* filters, AdminPage* adminPage) override;
    void onClose(CCObject* sender) override;

public:
    static SearchOptionsPopup* create(SentLevelFilters* filters, AdminPage* adminPage);
};

#endif
