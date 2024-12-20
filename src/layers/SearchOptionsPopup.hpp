#ifndef SEARCHOPTIONSPOPUP_HPP
#define SEARCHOPTIONSPOPUP_HPP
#include "../nodes/RangeOption.hpp"
#include "../types/SentLevelFilters.hpp"

using namespace geode::prelude;

// massive thanks to B1rtek
class SearchOptionsPopup final : public Popup<SentLevelFilters*> {
    SentLevelFilters* filters = nullptr;
    SentLevelFilters originalFilters;

    RangeOption* sendsOption = nullptr;
    RangeOption* starsOption = nullptr;
    RangeOption* featureOption = nullptr;

    bool setup(SentLevelFilters* filters) override;
    void onClose(CCObject* sender) override;

public:
    static SearchOptionsPopup* create(SentLevelFilters* filters);
};

#endif
