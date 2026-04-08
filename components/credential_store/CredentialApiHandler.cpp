#include "credential_store/CredentialApiHandler.hpp"
#include "credential_store/CredentialStore.hpp"
#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"
#include "esp_log.h"

using namespace http;

namespace credential_store {

static const char *TAG = "CredentialApiHandler";

CredentialApiHandler::CredentialApiHandler(CredentialStore &s)
    : store(s) {
    ESP_LOGD(TAG, "constructor");
}

void CredentialApiHandler::handle(http::HttpRequest& req,
                http::HttpResponse& res) {
	ESP_LOGD(TAG, "handle");
    const std::string &path = req.path();
	std::string action = extractAction(req.path());
	ESP_LOGD(TAG, "action '%s'", action.c_str());

    if (action == "list") {
		ESP_LOGD(TAG, "handleList");
        handleList(res);
        return;
    }
    if (path == "submit") {
		ESP_LOGD(TAG, "handleSubmit");
        handleSubmit(req, res);
        return;
    }
    if (path == "delete") {
		ESP_LOGD(TAG, "handleDelete");
        handleDelete(req, res);
        return;
    }
    if (path == "clear") {
		ESP_LOGD(TAG, "handleClear");
        handleClear(res);
        return;
    }
// TODO gotta do somethin better here for return
	ESP_LOGD(TAG, "Gotta do somethin better");
    return;
}

std::string CredentialApiHandler::extractAction(const char *uri) {
    std::string path(uri);
    auto pos = path.find_last_of('/');
    if (pos == std::string::npos || pos == path.length() - 1) {
        return {};  // no action found
    }
    return path.substr(pos + 1);
}

void CredentialApiHandler::handleList(HttpResponse &res) {
    res.jsonStatus("not_implemented");
}

void CredentialApiHandler::handleSubmit(const HttpRequest &req, HttpResponse &res) {
    res.jsonStatus("not_implemented");
}

void CredentialApiHandler::handleDelete(const HttpRequest &req, HttpResponse &res) {
    res.jsonStatus("not_implemented");
}

void CredentialApiHandler::handleClear(HttpResponse &res) {
    res.jsonStatus("not_implemented");
}

} // namespace core_api