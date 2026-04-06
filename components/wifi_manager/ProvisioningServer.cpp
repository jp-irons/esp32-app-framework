
#include "wifi_manager/ProvisioningServer.hpp"

#include "esp_log.h"
#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"
//#include "static_assets/StaticFileHandler.hpp"
#include "wifi_manager/WiFiContext.hpp"
#include "wifi_manager/WiFiStateMachine.hpp"

#include "static_assets/StaticFileHandler.hpp"
static_assert(std::is_base_of<http::HttpHandler, static_assets::StaticFileHandler>::value, "mismatch");
static_assert(std::is_trivially_destructible<static_assets::StaticFileHandler>::value == false, "just to force instantiation");



namespace wifi_manager {

static const char *TAG = "ProvisioningServer";

ProvisioningServer::ProvisioningServer(WiFiContext &ctx)
    : ctx(ctx)
    , server()
    , staticHandler("/provision", "index.html")
    , wifiHandler(ctx)
    , credentialHandler(*ctx.credentialStore) {}

ProvisioningServer::~ProvisioningServer() {
    stop();
}

bool ProvisioningServer::start() {
    ESP_LOGI(TAG, "Starting ProvisioningServer");

    server.start();

    if (!routesRegistered) {
        ESP_LOGD(TAG, "start() registering routes");
        server.addRoute("/assets/*", &staticHandler);
        // TODO implement handler for below
        //		server.addRoute("/api/wifi/*", &wifiHandler);
        //		server.addRoute("/api/credentials/*", &credentialHandler);

        // TODO Remove these legacy registrations
        //        server.registerHandler(http::HttpMethod::Get, "/provision/*", this, &ProvisioningServer::handleStaticFile);
        //
        //        server.registerHandler(http::HttpMethod::Post, "/provision/submit", this, &ProvisioningServer::handleSubmit);
        //
        //        server.registerHandler(http::HttpMethod::Get, "/provision/status", this, &ProvisioningServer::handleStatus);
        //
        //        server.registerHandler(http::HttpMethod::Get, "/provision/scan", this, &ProvisioningServer::handleScan);
        //
        routesRegistered = true;
    }

    return true;
}

void ProvisioningServer::stop() {
    ESP_LOGI(TAG, "Stopping ProvisioningServer");
    server.stop();
}

// TODO remove legacy handlers
// ------------------------------------------------------------
// Static file handler
// ------------------------------------------------------------
framework::Result ProvisioningServer::handleStaticFile(http::HttpRequest &req, http::HttpResponse &res) {
    ESP_LOGD(TAG, "handleStaticFile not implemented");
    //    auto file = staticRouter.serve(req.path());
    //
    //    if (!file.found) {
    //        return framework::Result::NotFound;
    //    }
    //    //    res.setContentTypeFromExtension(file.extension);
    //    res.send(file.data, file.size);
    return framework::Result::Ok;
}

// ------------------------------------------------------------
// Submit credentials
// ------------------------------------------------------------
framework::Result ProvisioningServer::handleSubmit(http::HttpRequest &req, http::HttpResponse &res) {
    ESP_LOGD(TAG, "handleSubmit not implemented");
    //    auto ssid = req.formField("ssid");
    //    auto pass = req.formField("password");
    //
    //    if (!ssid || !pass) {
    //        return fw::Result::BadRequest;
    //    }
    //
    //    // Notify provisioning state machine
    //    ctx.stateMachine->credentialsSubmitted(ssid.value(), pass.value());
    //
    //    res.json("{\"status\":\"submitted\"}");
    res.json("{\"status\":\"status\"}");
    return framework::Result::Ok;
}

// ------------------------------------------------------------
// Status
// ------------------------------------------------------------
framework::Result ProvisioningServer::handleStatus(http::HttpRequest &req, http::HttpResponse &res) {
    ESP_LOGD(TAG, "handleSubmit not implemented");
    //    auto state = ctx.stateMachine->currentState();
    //    auto err   = ctx.stateMachine->lastError();
    //
    //    std::string json = "{\"state\":\"" + toString(state) +
    //                       "\",\"error\":\"" + toString(err) + "\"}";
    //
    //    res.json(json);
    res.json("{\"status\":\"status\"}");
    return framework::Result::Ok;
}

// ------------------------------------------------------------
// Scan results
// ------------------------------------------------------------
framework::Result ProvisioningServer::handleScan(http::HttpRequest &req, http::HttpResponse &res) {
    ESP_LOGD(TAG, "handleScan not implemented");
    //    auto results = ctx.stateMachine->scanResultsJson();
    //
    //    res.json(results);
    res.json("{\"status\":\"scan\"}");
    return framework::Result::Ok;
}

void ProvisioningServer::handle(http::HttpRequest &req, http::HttpResponse &res) {
    const std::string &path = req.path();

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