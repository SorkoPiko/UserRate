#include "API.hpp"

#include "Global.hpp"

EventListener<web::WebTask> API::downloadListener;
std::unique_ptr<web::WebRequest> API::currentRequest;
LoadLayer* API::loadLayer;
bool API::isLoading = true;

void API::sendPostRequest(const std::string& url, const matjson::Value& body, const std::function<void(const matjson::Value&)>& callback) {
    downloadListener.bind([callback] (web::WebTask::Event* e) {
        if (const auto res = e->getValue()) {
            if (!res->ok()) {
                matjson::Value ret;
                matjson::Value defaultVal;
                defaultVal["detail"] = "Malformed response";
                ret["error"] = res->json().unwrapOr(defaultVal)["detail"];
                callback(ret);
                return;
            }
            const auto data = res->json().unwrapOr(matjson::Value{});
            callback(data);
        } else if (e->isCancelled()) {
            matjson::Value ret;
            ret["error"] = "Request was cancelled.";
            callback(ret);
        }
    });

    currentRequest = std::make_unique<web::WebRequest>();
    currentRequest->bodyJSON(body);
    downloadListener.setFilter(currentRequest->post(url));
}

void API::sendGetRequest(const std::string& url, const std::function<void(const matjson::Value&)>& callback) {
    downloadListener.bind([callback] (web::WebTask::Event* e) {
        if (const auto res = e->getValue()) {
            if (!res->ok()) {
                matjson::Value ret;
                matjson::Value defaultVal;
                defaultVal["detail"] = "Malformed response";
                ret["error"] = res->json().unwrapOr(defaultVal)["detail"];
                callback(ret);
                return;
            }
            const auto data = res->json().unwrapOrDefault();
            callback(data);
        } else if (e->isCancelled()) {
            matjson::Value ret;
            ret["error"] = "Request was cancelled.";
            callback(ret);
        }
    });

    currentRequest = std::make_unique<web::WebRequest>();
    downloadListener.setFilter(currentRequest->get(url));
}

void API::showFailurePopup(const std::string &message) {
    FLAlertLayer::create(
        "UserRate Error",
        message,
        "OK"
    )->show();
}

matjson::Value API::getAuth() {
    const auto acc = GJAccountManager::sharedState();

    matjson::Value auth;
    auth["gjp2"] = acc->m_GJP2.c_str();
    auth["accountID"] = acc->m_accountID;

    return auth;
}

void API::reassignModerator(const int accountID, const bool promote, const std::function<void(bool)>& callback) {
    matjson::Value body;

    if (!GJAccountManager::sharedState()->m_accountID) {
        showFailurePopup("You must be logged in to perform this action.");
        callback(false);
        return;
    }

    body["promote"] = promote;
    body["accountID"] = accountID;
    body["auth"] = getAuth();

    if (!isLoading) {
        loadLayer = LoadLayer::create();
        loadLayer->show();
        log::info("created load layer");
    }

    sendPostRequest(SERVER_URL "/admin/reassign", body, [callback](const matjson::Value& data) {
        if (loadLayer) loadLayer->finished();

        if (data.contains("error")) {
            showFailurePopup(fmt::format("Failed to reassign moderator: {}", data["error"].asString().unwrapOrDefault()));
            callback(false);
        } else {
            getModerators([](bool) {});
            callback(true);
        }
    });
}

void API::getModerators(const std::function<void(bool)>& callback) {
    if (!isLoading) {
        loadLayer = LoadLayer::create();
        loadLayer->show();
        log::info("created load layer");
    }

    sendGetRequest(SERVER_URL "/mods", [callback](const matjson::Value& data) {
        if (loadLayer) loadLayer->finished();

        if (data.contains("error")) {
            queueInMainThread([] {Notification::create("UserRate failed to fetch moderators", NotificationIcon::Error)->show();});
            callback(false);
        } else {
            const auto global = Global::getInstance();

            global->setMods(data["mods"].as<std::vector<int>>().unwrapOrDefault());
            global->setAdmins(data["admins"].as<std::vector<int>>().unwrapOrDefault());

            callback(true);
        }
    });
}

void API::getSentLevels(const SentLevelSearchType sort, const int page, const std::function<void(bool)>& callback) {
    if (!GJAccountManager::sharedState()->m_accountID) {
        showFailurePopup("You must be logged in to perform this action.");
        callback(false);
        return;
    }

    const auto auth = getAuth();
    const auto url = fmt::format(SERVER_URL "/mod/sent?sort={}&page={}&accountID={}&gjp2={}",
        static_cast<int>(sort),
        page,
        auth["accountID"].asInt().unwrapOrDefault(),
        auth["gjp2"].asString().unwrapOrDefault());

    sendGetRequest(url, [this, callback, page](const matjson::Value& data) {
        if (data.contains("error")) {
            showFailurePopup(fmt::format("Failed to fetch sent levels: {}", data["error"].asString().unwrapOrDefault()));
            callback(false);
        } else {
            // glm->getOnlineLevels and delegate stuff here
            Global::getInstance()->setLevelPage(page, data["levels"].as<std::vector<matjson::Value>>().unwrapOrDefault());
            callback(true);
        }
    });
}