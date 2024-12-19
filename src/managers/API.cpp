#include "API.hpp"

#include "Global.hpp"
#include "../delegates/GLMDelegate.hpp"
#include "../types/SentLevel.hpp"

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
            const auto global = Global::get();

            global->setMods(data["mods"].as<std::vector<int>>().unwrapOrDefault());
            global->setAdmins(data["admins"].as<std::vector<int>>().unwrapOrDefault());

            callback(true);
        }
    });
}

void API::getSentLevels(const SentLevelFilters filters, const std::function<void(bool)>& callback) {
    if (!GJAccountManager::sharedState()->m_accountID) {
        showFailurePopup("You must be logged in to perform this action.");
        callback(false);
        return;
    }

    const auto auth = getAuth();
    const auto url = fmt::format(SERVER_URL "/mod/sent?sort={}&page={}&accountID={}&gjp2={}",
        static_cast<int>(filters.sort),
        filters.page,
        auth["accountID"].asInt().unwrapOrDefault(),
        auth["gjp2"].asString().unwrapOrDefault());

    sendGetRequest(url, [callback, filters](const matjson::Value& data) {
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