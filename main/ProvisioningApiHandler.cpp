#include "ProvisioningApiHandler.hpp"
#include "cJSON.h"
#include "esp_log.h"

static const char* TAG = "ProvisioningApiHandler";

ProvisioningApiHandler::ProvisioningApiHandler(ApplicationContext& ctx)
    : ctx(ctx)
{}

bool ProvisioningApiHandler::handles(const std::string& path) const {
    // Accept anything under /api/provisioning/*
    return path.rfind("/api/provision/", 0) == 0;
}

esp_err_t ProvisioningApiHandler::handle(httpd_req_t* req) {
    std::string path = req->uri;
    ESP_LOGI(TAG, "Handling API request: %s", path.c_str());
    if (path == "/api/provision/status")
        return handleStatus(req);

    return httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Unknown provisioning endpoint");
}

esp_err_t ProvisioningApiHandler::handleStatus(httpd_req_t* req) {
    auto& wifi = *ctx.wifiManager;   // ← FIXED

    cJSON* root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "state",
        wifi.stateToString(wifi.getState()).c_str());

    cJSON_AddBoolToObject(root, "connected", wifi.isConnected());

    cJSON_AddStringToObject(root, "ssid",
        wifi.getLastAttemptedSsid().c_str());

    cJSON_AddNumberToObject(root, "lastErrorReason",
        wifi.getLastErrorReason());

    char* json = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    httpd_resp_set_type(req, "application/json");
    esp_err_t res = httpd_resp_sendstr(req, json);
    free(json);

    return res;
}