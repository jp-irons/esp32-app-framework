#include "credential_store/CredentialApiHandler.hpp"

#include "common/Result.hpp"
#include "credential_store/CredentialStore.hpp"
#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"
#include "logger/Logger.hpp"

#include <cJson.h>

using namespace http;
using namespace common;

namespace credential_store {

static logger::Logger log{"CredentialApiHandler"};

CredentialApiHandler::CredentialApiHandler(CredentialStore &s)
    : store(s) {
    log.debug("constructor");
}

Result CredentialApiHandler::handle(http::HttpRequest &req, http::HttpResponse &res) {
    log.debug("handle");
    const std::string &path = req.path();
    std::string action = extractAction(req.path());
    log.debug("action '%s'", action.c_str());

    if (action == "list") {
        log.info("handleList");
        return handleList(res);
    }
    if (action == "submit") {
        log.info("handleSubmit");
        return handleSubmit(req, res);
    }
    if (action == "delete") {
        log.info("handleDelete");
        return handleDelete(req, res);
    }
    if (action == "clear") {
        log.debug("handleClear");
        return handleClear(res);
    }
    if (action == "clearNvs") {
        log.info("handleClearNvs");
        return handleClearNvs(res);
    }
    log.error("handle action '%s' unsupported", action.c_str());
    return Result::Unsupported;
}

std::string CredentialApiHandler::extractAction(const char *uri) {
    std::string path(uri);
    auto pos = path.find_last_of('/');
    if (pos == std::string::npos || pos == path.length() - 1) {
        return {}; // no action found
    }
    return path.substr(pos + 1);
}

Result CredentialApiHandler::handleList(HttpResponse &res) {
    std::vector<WiFiCredential> entries;

    store.loadAllSortedByPriority(entries); // <-- correct API usage

    // Create the top-level JSON array
    cJSON *root = cJSON_CreateArray();
    if (!root) {
        return Result::InternalError;
    }

    for (const auto &c : entries) {
        cJSON *obj = cJSON_CreateObject();
        if (!obj) {
            cJSON_Delete(root);
			return Result::InternalError;
        }

        // Add fields
        cJSON_AddStringToObject(obj, "ssid", c.ssid.c_str());
        cJSON_AddNumberToObject(obj, "priority", c.priority);

        // Append to array
        cJSON_AddItemToArray(root, obj);
    }

    // Convert to string (pretty = false)
    char *json_response = cJSON_PrintUnformatted(root);

    // Free the cJSON tree; the printed string is independent
    cJSON_Delete(root);
	if (!json_response) {
	    return Result::InternalError;
	}
	Result result = res.json(json_response);
	cJSON_free(json_response);
	return result;
}

Result CredentialApiHandler::handleSubmit(const HttpRequest &req, HttpResponse &res) {
    res.jsonStatus("not_implemented");
    return Result::Unsupported;
}

Result CredentialApiHandler::handleDelete(const HttpRequest &req, HttpResponse &res) {
    res.jsonStatus("not_implemented");
    return Result::Unsupported;
}

Result CredentialApiHandler::handleClear(HttpResponse &res) {
    log.info("Clearing all credentials");
    store.clear();
    return res.jsonStatus("ok");
}

Result CredentialApiHandler::handleClearNvs(HttpResponse &res) {
    res.jsonStatus("not_implemented");
    return Result::Unsupported;
}

} // namespace credential_store