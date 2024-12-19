#ifndef SENTLEVELCELL_HPP
#define SENTLEVELCELL_HPP

#include "../types/SentLevel.hpp"

using namespace geode::prelude;

class SentLevelCell final : public CCLayer {
    bool init() override;

public:
    static SentLevelCell* create(SentLevel level, float width);
};

#endif
