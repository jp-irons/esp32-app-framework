#include "wifi_manager/WiFiApiHandler.hpp"

#include "esp_log.h"
#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"
#include "wifi_manager/WiFiContext.hpp"

using namespace http;

namespace wifi_manager {

static const char *TAG = "WiFiApiHandler";

WiFiApiHandler::WiFiApiHandler(WiFiContext &w)
    : wifiCtx(w) {
    ESP_LOGD(TAG, "constructor");
}


// handle requests not handled elsewhere
bool WiFiApiHandler::handle(http::HttpRequest &req, http::HttpResponse &res) {
	ESP_LOGD(TAG, "handle");
	const std::string &path = req.path();
	std::string action = extractAction(req.path());
	ESP_LOGD(TAG, "action '%s'", action.c_str());

    if (action == "scan") {
        return handleScan(res);
    }
//    if (path == "/api/wifi/status") {
//        handleStatus(res);
//        return;
//    }
//    if (path == "/api/wifi/connect") {
//        handleConnect(req, res);
//        return true;
//    }
//    if (path == "/api/wifi/disconnect") {
//        handleDisconnect(res);
//        return true;
//    }
//
    return false;
}

std::string WiFiApiHandler::extractAction(const char *uri) {
    std::string path(uri);
    auto pos = path.find_last_of('/');
    if (pos == std::string::npos || pos == path.length() - 1) {
        return {};  // no action found
    }
    return path.substr(pos + 1);
}

bool WiFiApiHandler::handleScan(HttpResponse &res) {
    res.jsonStatus("not_implemented");
	return false;
}

bool WiFiApiHandler::handleStatus(HttpResponse &res) {
    res.jsonStatus("not_implemented");
	return false;
}

bool WiFiApiHandler::handleConnect(const HttpRequest &req, HttpResponse &res) {
    res.jsonStatus("not_implemented");
	return false;
}

bool WiFiApiHandler::handleDisconnect(HttpResponse &res) {
    res.jsonStatus("not_implemented");
	return false;
}

} // namespace core_api