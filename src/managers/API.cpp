#include "API.hpp"

#include "Global.hpp"
#include "../layers/LoadLayer.hpp"

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

void API::reassignModerator(const int accountID, const bool promote, const std::function<void(bool)>& callback) {
    matjson::Value body;
    const auto acc = GJAccountManager::sharedState();

    if (!acc->m_accountID) {
        showFailurePopup("You must be logged in to perform this action.");
        callback(false);
        return;
    }

    body["promote"] = promote;
    body["accountID"] = accountID;
    body["gjp2"] = acc->m_GJP2.c_str();
    body["ownerID"] = acc->m_accountID;

    if (!isLoading) {
        loadLayer = LoadLayer::create();
        loadLayer->show();
        log::info("created load layer");
    }

    sendPostRequest(SERVER_URL "/admin/reassign", body, [callback](const matjson::Value& data) {
        if (loadLayer) {
            loadLayer->finished();
        }

        if (data.contains("error")) {
            showFailurePopup(fmt::format("Failed to reassign moderator: {}", data["error"].asString().unwrapOrDefault()));
            callback(false);
        }
        else {
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
        if (loadLayer) {
            loadLayer->finished();
        }

        if (data.contains("error")) {
            showFailurePopup(fmt::format("Failed to get moderators: {}", data["error"].asString().unwrapOrDefault()));
            callback(false);
        } else {
            const auto global = Global::getInstance();

            global->setMods(data["mods"].as<std::vector<int>>().unwrapOrDefault());
            global->setAdmins(data["admins"].as<std::vector<int>>().unwrapOrDefault());

            callback(true);
        }
    });
}
