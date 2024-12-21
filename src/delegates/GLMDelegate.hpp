#ifndef GLMDELEGATE_HPP
#define GLMDELEGATE_HPP

using namespace geode::prelude;

class GLMDelegate : public LevelManagerDelegate {
    std::function<void(CCArray*, char const*, int)> callback;

    void loadLevelsFinished(CCArray* p0, char const* p1) override {callback(p0, p1, -1);}
    void loadLevelsFinished(CCArray* p0, char const* p1, const int p2) override {callback(p0, p1, p2);}
    void loadLevelsFailed(char const* p0) override {callback(nullptr, p0, -1);}
    void loadLevelsFailed(char const* p0, const int p1) override {callback(nullptr, p0, p1);}

public:
    void setCallback(const std::function<void(CCArray*, char const*, int)>& externalCallback) {callback = externalCallback;}
};

#endif
