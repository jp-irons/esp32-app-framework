
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

ProvisioningServer::ProvisioningServer(WiFiContext &ctx, WiFiApiHandler &wifiApi,
                                       credential_store::CredentialApiHandler &credentialApi,
                                       device::DeviceApiHandler &deviceHandler)
    : ctx(ctx)
    , server()
    , staticHandler("/provision", "index.html")
    , fallbackHandler("/", "index.html")
    , wifiHandler(wifiApi)
    , credentialHandler(credentialApi) {
    log.debug("constructor");
}

ProvisioningServer::~ProvisioningServer() {
    log.info("destructor");
    stop();
}

bool ProvisioningServer::start() {
    log.info("Starting ProvisioningServer");

    server.start();

    if (!routesRegistered) {
        log.debug("start() registering routes");
        server.addRoutes(ctx.rootUri + "/credentials/*", &credentialHandler);
        // TODO change to addRoutes
        server.addPostRoute(ctx.rootUri + "/device/*", &deviceHandler);
        server.addGetRoute(ctx.rootUri + "/wifi/*", &wifiHandler);
        //		server.addGetRoute(ctx.rootUri + "/provision/*", this);
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
    std::string action = extractAction(req.path());
    log.debug("action '%s'", action.c_str());

    //	if (action == "status") {
    //	    log.debug("action status matched");
    //	    return handleStatus(res);
    //	}
    //
    //    if (path == "/provision/status") {
    //        return handleStatus(req, res);
    //    }
    //
    //    if (path == "/provision/reset") {
    //    if (path == "/provision/retry") {
    //
    // fallback: serve provisioning UI
    return staticHandler.handle(req, res);
}

} // namespace wifi_manager