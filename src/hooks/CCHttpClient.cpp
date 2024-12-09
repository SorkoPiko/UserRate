#include <Geode/modify/CCHttpClient.hpp>

using namespace geode::prelude;


class $modify(RateCCHttpClient, CCHttpClient) {
    void send(CCHttpRequest* request) {
#define SERVER_URL "http://localhost:8080"
        if (request->getUrl() == "https://www.boomlings.com/database/suggestGJStars20.php") request->setUrl(SERVER_URL "/mod/suggest");
        if (request->getUrl() == "https://www.boomlings.com/database/rateGJDemon21.php") request->setUrl(SERVER_URL "/demon");
        CCHttpClient::send(request);
    }
};