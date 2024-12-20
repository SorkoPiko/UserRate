#include "API.hpp"

#include "Global.hpp"
#include "../delegates/GLMDelegate.hpp"
#include "../types/SentLevel.hpp"

std::string API::token;
EventListener<web::WebTask> API::downloadListener;
std::unique_ptr<web::WebRequest> API::currentRequest;
EventListener<web::WebTask> API::tokenListener;
std::unique_ptr<web::WebRequest> API::tokenRequest;
LoadLayer* API::loadLayer;
bool API::isLoading = true;

void API::sendPostRequest(const std::string& url, const matjson::Value& body, const bool authRequired, const std::function<void(const matjson::Value&)>& callback) {
    auto retryCallback = [url, body, authRequired, callback](const matjson::Value& data) {
        if (data.contains("error") && data["error"].asString().unwrapOrDefault() == "Unauthorized") {
            getToken([url, body, authRequired, callback](const bool success) {
                if (success) {
                    sendPostRequest(url, body, authRequired, callback);
                } else {
                    matjson::Value ret;
                    ret["error"] = "Failed to refresh token.";
                    callback(ret);
                }
            });
        } else {
            callback(data);
        }
    };

    downloadListener.bind([callback, retryCallback] (web::WebTask::Event* e) {
        if (const auto res = e->getValue()) {
            if (!res->ok()) {
                matjson::Value ret;
                matjson::Value defaultVal;
                defaultVal["detail"] = "Malformed response";
                ret["error"] = res->json().unwrapOr(defaultVal)["detail"];
                if (res->code() == 401) {
                    matjson::Value unauthorized;
                    unauthorized["error"] = "Unauthorized";
                    retryCallback(unauthorized);
                } else {
                    callback(ret);
                }
                return;
            }
            const auto data = res->json().unwrapOr(matjson::Value{});
            retryCallback(data);
        } else if (e->isCancelled()) {
            matjson::Value ret;
            ret["error"] = "Request was cancelled.";
            callback(ret);
        }
    });

    currentRequest = std::make_unique<web::WebRequest>();
    currentRequest->bodyJSON(body);
    if (authRequired) {
        if (token.empty()) token = Mod::get()->getSavedValue<std::string>("token", "");
        currentRequest->header("Authorization", fmt::format("Bearer {}", token));
    }
    downloadListener.setFilter(currentRequest->post(url));
}

void API::sendGetRequest(const std::string& url, const bool authRequired, const std::function<void(const matjson::Value&)>& callback) {
    auto retryCallback = [url, authRequired, callback](const matjson::Value& data) {
        if (data.contains("error") && data["error"].asString().unwrapOrDefault() == "Unauthorized") {
            getToken([url, authRequired, callback](const bool success) {
                if (success) {
                    sendGetRequest(url, authRequired, callback);
                } else {
                    matjson::Value ret;
                    ret["error"] = "Failed to refresh token.";
                    callback(ret);
                }
            });
        } else {
            callback(data);
        }
    };

    downloadListener.bind([callback, retryCallback] (web::WebTask::Event* e) {
        if (const auto res = e->getValue()) {
            if (!res->ok()) {
                matjson::Value ret;
                matjson::Value defaultVal;
                defaultVal["detail"] = "Malformed response";
                ret["error"] = res->json().unwrapOr(defaultVal)["detail"];
                if (res->code() == 401) {
                    matjson::Value unauthorized;
                    unauthorized["error"] = "Unauthorized";
                    retryCallback(unauthorized);
                } else {
                    callback(ret);
                }
                return;
            }
            const auto data = res->json().unwrapOrDefault();
            retryCallback(data);
        } else if (e->isCancelled()) {
            matjson::Value ret;
            ret["error"] = "Request was cancelled.";
            callback(ret);
        }
    });

    currentRequest = std::make_unique<web::WebRequest>();
    if (authRequired) {
        if (token.empty()) token = Mod::get()->getSavedValue<std::string>("token", "");
        currentRequest->header("Authorization", fmt::format("Bearer {}", token));
    }
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

void API::getToken(const std::function<void(bool)>& callback) {
    if (!GJAccountManager::sharedState()->m_accountID) {
        showFailurePopup("You must be logged in to perform this action.");
        callback(false);
        return;
    }

    tokenListener.bind([callback] (web::WebTask::Event* e) {
        if (const auto res = e->getValue()) {
            if (!res->ok()) {
                matjson::Value ret;
                matjson::Value defaultVal;
                defaultVal["detail"] = "Malformed response";
                const auto error = res->json().unwrapOr(defaultVal)["detail"].asString().unwrapOrDefault();
                showFailurePopup(fmt::format("Failed to fetch token: {}", error));
                return;
            }
            const auto data = res->json().unwrapOr(matjson::Value{});
            token = data["access_token"].asString().unwrapOrDefault();
            Mod::get()->setSavedValue("token", token);
            callback(true);
        } else if (e->isCancelled()) {
            showFailurePopup("Failed to fetch token: Request was cancelled.");
        }
    });

    tokenRequest = std::make_unique<web::WebRequest>();
    tokenRequest->bodyJSON(getAuth());
    tokenListener.setFilter(tokenRequest->post(SERVER_URL "/token"));
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
    }

    sendPostRequest(SERVER_URL "/admin/reassign", body,true,  [callback](const matjson::Value& data) {
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
    }

    sendGetRequest(SERVER_URL "/mods", false, [callback](const matjson::Value& data) {
        if (loadLayer) loadLayer->finished();

        if (data.contains("error")) {
            queueInMainThread([] {Notification::create("UserRate failed to fetch moderators", NotificationIcon::Error)->show();});
            callback(false);
        } else {
            const auto global = Global::get();

            global->setMods(data["mods"].as<std::vector<int>>().unwrapOrDefault());
            global->setAdmins(data["admins"].as<std::vector<int>>().unwrapOrDefault());

            callback(true);
        }
    });
}

void API::getSentLevels(const SentLevelFilters& filters, const std::function<void(bool)>& callback) {
    if (!GJAccountManager::sharedState()->m_accountID) {
        showFailurePopup("You must be logged in to perform this action.");
        callback(false);
        return;
    }

    const auto auth = getAuth();
    std::ostringstream url;
    url << SERVER_URL << "/mod/sent?sort=" << filters.sort
        << "&page=" << filters.page
        << "&accountID=" << auth["accountID"].asInt().unwrapOrDefault()
        << "&gjp2=" << auth["gjp2"].asString().unwrapOrDefault();

    if (filters.minSends > 0) {
        url << "&min_send_count=" << filters.minSends;
    }
    if (filters.maxSends.has_value()) {
        url << "&max_send_count=" << filters.maxSends.value();
    }
    if (filters.minAvgStars > 0) {
        url << "&min_avg_stars=" << filters.minAvgStars;
    }
    if (filters.maxAvgStars < 10) {
        url << "&max_avg_stars=" << filters.maxAvgStars;
    }
    if (filters.minAvgFeature > 0) {
        url << "&min_avg_feature=" << filters.minAvgFeature;
    }
    if (filters.maxAvgFeature < 4) {
        url << "&max_avg_feature=" << filters.maxAvgFeature;
    }

    sendGetRequest(url.str(), true, [callback, filters](const matjson::Value& data) {
        if (data.contains("error")) {
            showFailurePopup(fmt::format("Failed to fetch sent levels: {}", data["error"].asString().unwrapOrDefault()));
            callback(false);
        } else {
            const auto global = Global::get();
            const auto levels = data["levels"].as<std::vector<SentLevel>>().unwrapOrDefault();

            global->setLevelPage(filters.page, levels);

            std::vector<int> levelIDs;
            levelIDs.reserve(levels.size());
            std::ranges::transform(levels, std::back_inserter(levelIDs), [](const SentLevel& level) {return level.id;});

            const auto remaining = global->allCached(levelIDs);
            if (remaining.empty()) {
                callback(true);
                return;
            }

            const auto glm = GameLevelManager::get();
            auto delegate = GLMDelegate();
            delegate.setCallback([callback, global](CCArray* delegateLevels, const char* error, const int error2) {
                if (delegateLevels) {
                    for (GJGameLevel* level : CCArrayExt<GJGameLevel*>(delegateLevels)) {
                        global->addLevel(level->m_levelID, level->m_levelName, level->isPlatformer());
                    }
                    callback(true);
                } else {
                    showFailurePopup(fmt::format("Failed to fetch sent levels: {} ({})", error, error2));
                    callback(false);
                }
            });

            glm->m_levelManagerDelegate = &delegate;
            glm->getOnlineLevels(GJSearchObject::create(SearchType::Type26, fmt::format("{}", fmt::join(remaining, ","))));
        }
    });
}