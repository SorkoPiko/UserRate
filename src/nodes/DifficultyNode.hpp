#ifndef DIFFICULTYNODE_HPP
#define DIFFICULTYNODE_HPP
#include "../types/RatedLevel.hpp"

using namespace geode::prelude;

class DifficultyNode final : public CCNode {
    GJDifficultySprite* diffSprite = nullptr;
    CCSprite* starSprite = nullptr;
    CCLabelBMFont* starLabel = nullptr;

    bool init(int stars, const GJDifficultyName& length, const GJFeatureState& feature, bool isPlatformer);

public:
    static DifficultyNode* create(int stars, const GJDifficultyName& length, const GJFeatureState& feature, bool isPlatformer);

    static int getDifficultyForStars(int stars);
    static int getDifficultyForDemon(DemonDifficulty demon);
};

#endif
