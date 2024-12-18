#ifndef LOADLAYER_HPP
#define LOADLAYER_HPP

using namespace geode::prelude;

// thanks firee (https://github.com/FireMario211/Object-Workshop/blob/main/src/ui/auth/AuthLoadLayer.hpp)

class LoadLayer final : public FLAlertLayer {
    LoadingCircle* m_loadingCircle = nullptr;
    bool init() override;

public:
    void finished();
    static LoadLayer* create();
};


#endif
