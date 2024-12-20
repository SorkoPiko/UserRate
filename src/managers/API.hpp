#ifndef API_HPP
#define API_HPP

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

    static void sendPostRequest(const std::string& url, const matjson::Value& body, bool authRequired, const std::function<void(const matjson::Value&)>& callback);
    static void sendGetRequest(const std::string& url, bool authRequired, const std::function<void(const matjson::Value&)>& callback);

    static void showFailurePopup(const std::string& message);
    static matjson::Value getAuth();

    ~API() {GameLevelManager::get()->m_levelManagerDelegate = nullptr;}

public:
    static void loadingComplete() {isLoading = false;}

    static void getToken(const std::function<void(bool)>& callback);
    static void reassignModerator(int accountID, bool promote, const std::function<void(bool)>& callback);
    static void getModerators(const std::function<void(bool)>& callback);
    static void getSentLevels(const SentLevelFilters& filters, const std::function<void(bool)>& callback);
    static void clearLevelSends(int levelID, const std::function<void(bool)>& callback);
    static void rateLevel(int levelID, int stars, int feature, const std::function<void(bool)>& callback);
    static void checkRatedLevels(const std::vector<int>& levelIDs, const std::function<void(bool)>& callback);
};



#endif
