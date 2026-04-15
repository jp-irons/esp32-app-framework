#include "wifi_manager/RuntimeServer.hpp"

#include "credential_store/CredentialApiHandler.hpp"
#include "device/DeviceApiHandler.hpp"
#include "logger/Logger.hpp"
#include "wifi_manager/WiFiApiHandler.hpp"
#include "wifi_manager/WiFiContext.hpp"
#include "wifi_manager/WiFiStateMachine.hpp"

namespace wifi_manager {

using namespace common;

static logger::Logger log{"RuntimeServer"};

RuntimeServer::RuntimeServer(WiFiContext &ctx, WiFiApiHandler &wifiApi,
                             credential_store::CredentialApiHandler &credentialApi,
                             device::DeviceApiHandler &deviceHandler)
    : ctx(ctx)
    , server()
    , staticHandler("/", "index.html")
    , fallbackHandler("/", "index.html")
    , wifiHandler(wifiApi)
    , credentialHandler(credentialApi)
    , deviceHandler(deviceHandler) {
    log.debug("constructor");
}
RuntimeServer::~RuntimeServer() {
    log.info("destructor");
    stop();
}

bool RuntimeServer::start() {
    log.info("Starting RuntimeServer");

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

void RuntimeServer::stop() {
    log.debug("Stopping RuntimeServer");
    server.stop();
}

// handle requests not handled elsewhere
Result RuntimeServer::handle(http::HttpRequest &req, http::HttpResponse &res) {
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