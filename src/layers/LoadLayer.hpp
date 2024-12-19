#ifndef LOADLAYER_HPP
#define LOADLAYER_HPP

using namespace geode::prelude;

// thanks firee (github.com/FireMario211/Object-Workshop/blob/dad5b6aa4d9190caaf513fe0abea6eb3cbfbcb22/src/ui/auth/AuthLoadLayer.hpp)

class LoadLayer final : public FLAlertLayer {
    LoadingCircle* m_loadingCircle = nullptr;
    bool init() override;

public:
    void finished();
    static LoadLayer* create();
};

#endif
