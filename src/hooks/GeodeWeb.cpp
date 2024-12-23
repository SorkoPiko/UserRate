#include <Geode/utils/web.hpp>

#include "../managers/Global.hpp"
#include "../managers/API.hpp"

using namespace geode::prelude;

web::WebTask WebRequest_send(web::WebRequest* request, const std::string_view methodView, const std::string_view urlView) {
    if (!Global::get()->isCustomRating()) return request->send(methodView, urlView);
    if (urlView.find("suggestGJStars20.php") != std::string::npos) return request->send(methodView, SERVER_URL "/suggest");
    if (urlView.find("rateGJDemon21.php") != std::string::npos) return request->send(methodView, SERVER_URL "/demon");

    return request->send(methodView, urlView);
}

$execute {
    if (auto hook = Mod::get()->hook(
        reinterpret_cast<void*>(getNonVirtual(&web::WebRequest::send)),
        &WebRequest_send,
        "geode::web::WebRequest::send",
        tulip::hook::TulipConvention::Thiscall
    ); hook.err()) log::error("{}", hook.unwrapErr());
}