#include <Geode/modify/CCHttpClient.hpp>

#include "../managers/Global.hpp"

using namespace geode::prelude;

#define SERVER_URL "http://127.0.0.1:8000"

class $modify(RateCCHttpClient, CCHttpClient) {
    void send(CCHttpRequest* request) {
        if (!Global::getInstance()->isCustomRating()) return CCHttpClient::send(request);
        if (const std::string url = request->getUrl(); url.find("suggestGJStars20.php") != std::string::npos) request->setUrl(SERVER_URL "/mod/suggest");
        if (const std::string url = request->getUrl(); url.find("rateGJDemon21.php") != std::string::npos) request->setUrl(SERVER_URL "/demon");
        log::info("custom rate request: {}", request->getUrl());
        CCHttpClient::send(request);
    }
};