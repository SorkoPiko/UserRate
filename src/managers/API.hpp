#ifndef API_HPP
#define API_HPP

#define SERVER_URL "https://userrate.sorkopiko.com"
//#define SERVER_URL "http://127.0.0.1:8000"

#include <Geode/utils/web.hpp>

#include "../layers/LoadLayer.hpp"
#include "../types/SentLevelFilters.hpp"

using namespace geode::prelude;

class API {
    static std::string token;
    static EventListener<web::WebTask> downloadListener;
    static std::unique_ptr<web::WebRequest> currentRequest;
    static EventListener<web::WebTask> tokenListener;
    static std::unique_ptr<web::WebRequest> tokenRequest;
    static LoadLayer* loadLayer;
    static bool isLoading;

    static void sendPostRequest(const std::string& url, const matjson::Value& body, bool authRequired, const std::function<void(const matjson::
                                    Value&)>& callback, EventListener<web::WebTask>& listener);
    static void sendGetRequest(const std::string& url, bool authRequired, const std::function<void(const matjson::Value&)>& callback, EventListener
                               <web::WebTask>& listener);

    static void showFailurePopup(const std::string& message);
    static matjson::Value getAuth();

    ~API() {GameLevelManager::get()->m_levelManagerDelegate = nullptr;}

public:
    static void loadingComplete() {isLoading = false;}

    // these callbacks are NOT SAFE
    // meaning they can be called after the caller has been destroyed
    // i couldn't find a way to fix this so i just manually check if the relevant objects are still alive
    static void getToken(const std::function<void(bool)>& callback);
    static void reassignModerator(int accountID, bool promote, const std::function<void(bool)>& callback, EventListener<web::WebTask>& listener);
    static void getModerators(const std::function<void(bool)>& callback, EventListener<web::WebTask>& listener);
    static void getModerators(const std::function<void(bool)>& callback);
    static void getSentLevels(const SentLevelFilters& filters, const std::function<void(bool)>& callback, EventListener<web::WebTask>& listener);
    static void clearLevelSends(int levelID, const std::function<void(bool)>& callback, EventListener<web::WebTask>& listener);
    static void rateLevel(int levelID, int stars, int feature, const std::function<void(bool)>& callback, EventListener<web::WebTask>& listener);
    static void derateLevel(int levelID, const std::function<void(bool)>& callback, EventListener<web::WebTask>& listener);
    static void checkRatedLevels(const std::vector<int>& levelIDs, const std::function<void(bool)>& callback, EventListener<web::WebTask>& listener);
    static void getLatestRates(const std::function<void(bool)>& callback, EventListener<web::WebTask>& listener);
};



#endif
