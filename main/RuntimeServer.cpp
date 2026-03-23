#include "RuntimeServer.hpp"
#include "esp_log.h"

static const char* TAG = "RuntimeServer";

RuntimeServer::RuntimeServer(ApplicationContext& ctx)
    : ctx(ctx)
{
    credentialApi = std::make_unique<CredentialApiHandler>(ctx);
    wifiApi = std::make_unique<WiFiApiHandler>(ctx);
}

esp_err_t RuntimeServer::start() {
    if (server) {
        ESP_LOGW(TAG, "RuntimeServer already running");
        return ESP_OK;
    }

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_LOGI(TAG, "Starting RuntimeServer on port %d", config.server_port);

    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP server");
        return ESP_FAIL;
    }

    // -------------------------------
    // API router: /api/*
    // -------------------------------
    httpd_uri_t api_get = {
        .uri      = "/api/*",
        .method   = HTTP_GET,
        .handler  = &RuntimeServer::dispatchApi,
        .user_ctx = this
    };
    httpd_register_uri_handler(server, &api_get);

    httpd_uri_t api_post = {
        .uri      = "/api/*",
        .method   = HTTP_POST,
        .handler  = &RuntimeServer::dispatchApi,
        .user_ctx = this
    };
    httpd_register_uri_handler(server, &api_post);

    httpd_uri_t api_delete = {
        .uri      = "/api/*",
        .method   = HTTP_DELETE,
        .handler  = &RuntimeServer::dispatchApi,
        .user_ctx = this
    };
    httpd_register_uri_handler(server, &api_delete);

    // -------------------------------
    // Static file fallback: /* 
    // -------------------------------
    httpd_uri_t files = {
        .uri      = "/*",
        .method   = HTTP_GET,
        .handler  = [](httpd_req_t* req) {
            return serveEmbedded(req, "/");   // <-- BASE PATH HERE
        },
        .user_ctx = this
    };
    httpd_register_uri_handler(server, &files);

    return ESP_OK;
}

esp_err_t RuntimeServer::stop() {
    if (server) {
        ESP_LOGI(TAG, "Stopping RuntimeServer");
        httpd_stop(server);
        server = nullptr;
    }
    return ESP_OK;
}

// ---------------------------------------------------------
// API dispatcher
// ---------------------------------------------------------
esp_err_t RuntimeServer::dispatchApi(httpd_req_t* req) {
    auto* self = static_cast<RuntimeServer*>(req->user_ctx);
    std::string path = req->uri;

    ESP_LOGI(TAG, "API request: %s", path.c_str());

    if (self->credentialApi->handles(path))
        return self->credentialApi->handle(req);

    if (self->wifiApi->handles(path))
        return self->wifiApi->handle(req);

    return httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Unknown API endpoint");
}
