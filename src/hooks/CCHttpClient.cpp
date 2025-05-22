#include <Geode/modify/CCHttpClient.hpp>

#include "../managers/Global.hpp"
#include "../managers/API.hpp"

using namespace geode::prelude;

class $modify(RateCCHttpClient, CCHttpClient) {
    static void onModify(auto& self) {
        if (!self.setHookPriorityPre("CCHttpClient::send", Priority::VeryEarly)) log::warn("Failed to set hook priority.");
    }

    void send(CCHttpRequest* request) {
        if (!Global::get()->isCustomRating()) return CCHttpClient::send(request);
        const std::string url = request->getUrl();
        if (url.find("suggestGJStars20.php") != std::string::npos) request->setUrl(SERVER_URL "/suggest");
        if (url.find("rateGJDemon21.php") != std::string::npos) request->setUrl(SERVER_URL "/demon");

        CCHttpClient::send(request);
    }
};