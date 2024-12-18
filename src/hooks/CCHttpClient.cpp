#include <Geode/modify/CCHttpClient.hpp>

#include "../managers/Global.hpp"

using namespace geode::prelude;

class $modify(RateCCHttpClient, CCHttpClient) {
    void send(CCHttpRequest* request) {
        if (!Global::getInstance()->isCustomRating()) return CCHttpClient::send(request);
        if (const std::string url = request->getUrl(); url.find("suggestGJStars20.php") != std::string::npos) request->setUrl(SERVER_URL "/suggest");
        if (const std::string url = request->getUrl(); url.find("rateGJDemon21.php") != std::string::npos) request->setUrl(SERVER_URL "/demon");
        CCHttpClient::send(request);
    }
};