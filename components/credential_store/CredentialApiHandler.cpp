#include "credential_store/CredentialApiHandler.hpp"
#include "common/Result.hpp"
#include "credential_store/CredentialStore.hpp"
#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"
#include "logger/Logger.hpp"

using namespace http;

namespace credential_store {

static logger::Logger log{"CredentialApiHandler"};

CredentialApiHandler::CredentialApiHandler(CredentialStore &s)
    : store(s) {
    log.debug("constructor");
}

common::Result CredentialApiHandler::handle(http::HttpRequest& req,
                http::HttpResponse& res) {
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
	return common::Result::Unsupported;
}

std::string CredentialApiHandler::extractAction(const char *uri) {
    std::string path(uri);
    auto pos = path.find_last_of('/');
    if (pos == std::string::npos || pos == path.length() - 1) {
        return {};  // no action found
    }
    return path.substr(pos + 1);
}

common::Result CredentialApiHandler::handleList(HttpResponse &res) {
	res.jsonStatus("not_implemented");
	return common::Result::Unsupported;
}

common::Result CredentialApiHandler::handleSubmit(const HttpRequest &req, HttpResponse &res) {
    res.jsonStatus("not_implemented");
	return common::Result::Unsupported;
}

common::Result CredentialApiHandler::handleDelete(const HttpRequest &req, HttpResponse &res) {
    res.jsonStatus("not_implemented");
	return common::Result::Unsupported;
}

common::Result CredentialApiHandler::handleClear(HttpResponse &res) {
	log.info("Clearing all credentials");
	store.clear();
	return res.jsonStatus("ok");
}

common::Result CredentialApiHandler::handleClearNvs(HttpResponse &res) {
    res.jsonStatus("not_implemented");
	return common::Result::Unsupported;
}

} // namespace core_api