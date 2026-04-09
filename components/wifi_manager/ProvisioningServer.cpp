
#include "wifi_manager/ProvisioningServer.hpp"

#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"
#include "logger/Logger.hpp"
#include "static_assets/StaticFileHandler.hpp"
#include "wifi_manager/WiFiContext.hpp"
#include "wifi_manager/WiFiStateMachine.hpp"

namespace wifi_manager {
	
using namespace common;

static logger::Logger log{"ProvisioningServer"};

ProvisioningServer::ProvisioningServer(WiFiContext &ctx)
    : ctx(ctx)
    , server()
    // TODO sort this out properly?
    , staticHandler("/provision", "index.html")
    , fallbackHandler("/", "index.html")
    , wifiHandler(ctx)
    , credentialHandler(*ctx.credentialStore) {
		log.debug("constructor");
	}

ProvisioningServer::~ProvisioningServer() {
    stop();
}

bool ProvisioningServer::start() {
	log.info("Starting ProvisioningServer");

    server.start();

    if (!routesRegistered) {
        log.debug("start() registering routes");
        server.addGetRoute("/provision/*", &staticHandler);
		server.addGetRoute("/api/framework/credentials/*", &credentialHandler);
		server.addPostRoute("/api/framework/credentials/*", &credentialHandler);
		server.addDeleteRoute("/api/framework/credentials/*", &credentialHandler);
        server.addGetRoute("/api/framework/wifi/*", &wifiHandler);
        server.addGetRoute("/*", &fallbackHandler);

        routesRegistered = true;
    }

    return true;
}

void ProvisioningServer::stop() {
    log.debug("Stopping ProvisioningServer");
    server.stop();
}

// handle requests not handled elsewhere
Result ProvisioningServer::handle(http::HttpRequest &req, http::HttpResponse &res) {
    const std::string &path = req.path();
    log.debug("handle");

    //    if (path == "/provision/status") {
    //        return handleStatus(req, res);
    //    }
    //
    //    if (path == "/provision/reset") {
    //        return handleReset(req, res);
    //    }
    //
    //    if (path == "/provision/retry") {
    //        return handleRetry(req, res);
    //    }
    //
    // fallback: serve provisioning UI
    return staticHandler.handle(req, res);
}

} // namespace wifi_manager