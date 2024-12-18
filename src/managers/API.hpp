#ifndef API_HPP
#define API_HPP

#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class API {
    static EventListener<web::WebTask> downloadListener;
    static std::unique_ptr<web::WebRequest> currentRequest;

    static void sendPostRequest(const std::string& url, const matjson::Value& body, const std::function<void(const matjson::Value&)>& callback);
    static void sendGetRequest(const std::string& url, const std::function<void(const matjson::Value&)>& callback);

public:
    static void reassignModerator(int accountID, bool promote, const std::function<void(bool, const std::string&)>& callback);
    static void isModerator(int accountID, const std::function<void(bool, bool, const std::string&)>& callback);
};



#endif
