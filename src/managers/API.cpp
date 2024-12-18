#include "API.hpp"

#include "Global.hpp"

EventListener<web::WebTask> API::downloadListener;
std::unique_ptr<web::WebRequest> API::currentRequest;

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
            const auto data = res->json().unwrapOrDefault();
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

void API::reassignModerator(const int accountID, const bool promote, const std::function<void(bool, const std::string&)>& callback) {
    matjson::Value body;
    const auto acc = GJAccountManager::sharedState();

    if (!acc->m_accountID) {
        callback(false, "You must be logged in to perform this action.");
        return;
    }

    body["promote"] = promote;
    body["accountID"] = accountID;
    body["gjp2"] = acc->m_GJP2;
    body["ownerID"] = acc->m_accountID;

    sendPostRequest(SERVER_URL "/admin/reassign", body, [callback](const matjson::Value& data) {
        if (data.contains("error")) callback(false, data["error"].asString().unwrapOrDefault());
        else callback(true, data["message"].asString().unwrapOrDefault());
    });
}

// not sending any credentials as it's not giving any sensitive data, and all the features and endpoints this "unlocks" are protected, anyway
void API::isModerator(int accountID, const std::function<void(bool, bool, const std::string&)>& callback) {
    sendGetRequest(fmt::format(SERVER_URL "/checkmod?accountid={}", accountID), [callback](const matjson::Value& data) {
        if (data.contains("error")) callback(false, false, data["error"].asString().unwrapOrDefault());
        else callback(data["moderator"].asBool().unwrapOrDefault(), data["admin"].asBool().unwrapOrDefault(), "");
    });
}