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

bool CredentialApiHandler::handle(http::HttpRequest& req,
                http::HttpResponse& res) {
	ESP_LOGD(TAG, "handle");
    const std::string &path = req.path();
	std::string action = extractAction(req.path());
	ESP_LOGD(TAG, "action '%s'", action.c_str());

    if (action == "list") {
		ESP_LOGD(TAG, "handleList");
        return handleList(res);
    }
    if (path == "submit") {
		ESP_LOGD(TAG, "handleSubmit");
        return handleSubmit(req, res);
    }
    if (path == "delete") {
		ESP_LOGD(TAG, "handleDelete");
        return handleDelete(req, res);
    }
    if (path == "clear") {
		ESP_LOGD(TAG, "handleClear");
        return handleClear(res);
    }
    return false;
}

std::string CredentialApiHandler::extractAction(const char *uri) {
    std::string path(uri);
    auto pos = path.find_last_of('/');
    if (pos == std::string::npos || pos == path.length() - 1) {
        return {};  // no action found
    }
    return path.substr(pos + 1);
}

bool CredentialApiHandler::handleList(HttpResponse &res) {
    res.jsonStatus("not_implemented");
	return false;
}

bool CredentialApiHandler::handleSubmit(const HttpRequest &req, HttpResponse &res) {
    res.jsonStatus("not_implemented");
	return false;
}

bool CredentialApiHandler::handleDelete(const HttpRequest &req, HttpResponse &res) {
    res.jsonStatus("not_implemented");
	return false;
}

bool CredentialApiHandler::handleClear(HttpResponse &res) {
    res.jsonStatus("not_implemented");
	return false;
}

} // namespace core_api