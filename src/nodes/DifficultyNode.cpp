#include "DifficultyNode.hpp"

DifficultyNode* DifficultyNode::create(const int stars, const GJDifficultyName& length, const GJFeatureState& feature, const bool isPlatformer) {
    const auto ret = new DifficultyNode();
    if (ret->init(stars, length, feature, isPlatformer)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool DifficultyNode::init(const int stars, const GJDifficultyName& length, const GJFeatureState& feature, const bool isPlatformer) {
    if (!CCNode::init()) return false;

    diffSprite = GJDifficultySprite::create(getDifficultyForStars(stars), length);
    diffSprite->setID("difficulty-sprite");
    diffSprite->updateFeatureState(feature);
    diffSprite->setPositionY(5.f);
    addChild(diffSprite);

    starSprite = CCSprite::createWithSpriteFrameName(isPlatformer ? "moon_small01_001.png" : "star_small01_001.png");
    starSprite->setID("star-sprite");
    starSprite->setContentSize({12.5f, 12.5f});
    starSprite->setPosition({8.f, -25.f});
    addChild(starSprite);

    starLabel = CCLabelBMFont::create(std::to_string(stars).c_str(), "bigFont.fnt");
    starLabel->setID("star-label");
    starLabel->setScale(0.4f);
    starLabel->setAnchorPoint({1.f, 0.5f});
    starLabel->setPositionY(-25.f);
    addChild(starLabel);

    return true;
}

int DifficultyNode::getDifficultyForStars(const int stars) {
    switch (stars) {
        case 1: return -1;
        case 2: return 1;
        case 3: return 2;
        case 4:
        case 5: return 3;
        case 6:
        case 7: return 4;
        case 8:
        case 9: return 5;
        case 10: return 6;
        default: return 0;
    }
}
