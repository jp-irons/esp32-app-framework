#include "ProvisioningServer.hpp"
#include "CredentialStore.hpp"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_err.h"
#include <cstring>

namespace wifi_manager {
	
	
	static const char* TAG = "ProvisioningServer";

	ProvisioningServer::ProvisioningServer(WiFiContext* ctx)
	    : ctx(ctx)
	{
	    // No work here — constructor is intentionally minimal.
	}
	// --- Static handler struct definitions (Pattern 2) ---

	httpd_uri_t ProvisioningServer::uri_root = {
	    .uri      = "/",
	    .method   = HTTP_GET,
	    .handler  = &ProvisioningServer::handle_root_get,
	    .user_ctx = nullptr
	};

	httpd_uri_t ProvisioningServer::uri_scan = {
	    .uri      = "/scan",
	    .method   = HTTP_GET,
	    .handler  = &ProvisioningServer::handle_scan_get,
	    .user_ctx = nullptr
	};

	httpd_uri_t ProvisioningServer::uri_submit = {
	    .uri      = "/submit",
	    .method   = HTTP_POST,
	    .handler  = &ProvisioningServer::handle_submit_post,
	    .user_ctx = nullptr
	};


	httpd_uri_t ProvisioningServer::uri_status = {
	    .uri      = "/status",
	    .method   = HTTP_GET,
	    .handler  = &ProvisioningServer::handle_status_get,
	    .user_ctx = nullptr
	};
	
// Small helper: store pointer in httpd global user_ctx
// We avoid dynamic allocation; httpd server is created once per start().
ProvisioningServer::ProvisioningServer(credential_store::CredentialStore* s)
    : store_(s),
      current(ProvisioningState::Idle),
      complete(nullptr),
      failure(nullptr),
      server(nullptr)
{
}

void ProvisioningServer::setState(ProvisioningState s)
{
    current = s;
    ESP_LOGI(TAG, "State -> %s", toString(s));
}

bool ProvisioningServer::start()
{
    ESP_LOGI(TAG, "Starting provisioning");

    if (current != ProvisioningState::Idle) {
        ESP_LOGW(TAG, "Cannot start provisioning: not idle");
        return false;
    }

    // Begin provisioning workflow
    setState(ProvisioningState::StartingProvisioning);

    // Start AP
    if (!startAP()) {
        ESP_LOGE(TAG, "Failed to start AP");
        setState(ProvisioningState::ProvisioningComplete); // terminal failure state
        if (failure) failure();
        return false;
    }

    // Start HTTP server
    if (!startHttp()) {
        ESP_LOGE(TAG, "Failed to start HTTP server");
        stopAP();
        setState(ProvisioningState::ProvisioningComplete);
        if (failure) failure();
        return false;
    }

    // Provisioning is now active
    setState(ProvisioningState::Provisioning);
    return true;
}void ProvisioningServer::stop()
{
    ESP_LOGI(TAG, "Stopping provisioning");

    stopHttp();
    stopAP();

    setState(ProvisioningState::Idle);
}

bool ProvisioningServer::startAP()
{
    wifi_config_t ap_cfg = {};
    std::strcpy(reinterpret_cast<char*>(ap_cfg.ap.ssid), "ESP-Provisioning");
    ap_cfg.ap.ssid_len       = std::strlen("ESP-Provisioning");
    ap_cfg.ap.channel        = 1;
    ap_cfg.ap.max_connection = 4;
    ap_cfg.ap.authmode       = WIFI_AUTH_OPEN;

    esp_err_t err;

    err = esp_wifi_set_mode(WIFI_MODE_AP);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_set_mode failed: %d", err);
        return false;
    }

    err = esp_wifi_set_config(WIFI_IF_AP, &ap_cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_set_config failed: %d", err);
        return false;
    }

    err = esp_wifi_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_start failed: %d", err);
        return false;
    }

    ESP_LOGI(TAG, "AP started");
    return true;
}

void ProvisioningServer::stopAP()
{
    esp_err_t err = esp_wifi_stop();
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "esp_wifi_stop failed: %d", err);
    } else {
        ESP_LOGI(TAG, "AP stopped");
    }
}

bool ProvisioningServer::startHttp()
{
    if (server != nullptr) {
        ESP_LOGW(TAG, "HTTP server already running");
        return true;
    }

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 8;
    config.lru_purge_enable = true;

    esp_err_t err = httpd_start(&server, &config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "httpd_start failed: %d", err);
        server = nullptr;
        return false;
    }

    // Attach instance pointer to each handler
    uri_root.user_ctx   = this;
    uri_scan.user_ctx   = this;
    uri_submit.user_ctx = this;
    uri_status.user_ctx = this;

    httpd_register_uri_handler(server, &uri_root);
    httpd_register_uri_handler(server, &uri_scan);
    httpd_register_uri_handler(server, &uri_submit);
    httpd_register_uri_handler(server, &uri_status);

    ESP_LOGI(TAG, "HTTP server started");
    return true;
}

void ProvisioningServer::stopHttp()
{
    if (server != nullptr) {
        httpd_stop(server);
        server = nullptr;
        ESP_LOGI(TAG, "HTTP server stopped");
    }
}

ProvisioningServer* ProvisioningServer::fromReq(httpd_req_t* req)
{
    return static_cast<ProvisioningServer*>(req->user_ctx);
}

// --- HTTP handlers ---

esp_err_t ProvisioningServer::handle_root_get(httpd_req_t* req)
{
    ProvisioningServer* self = fromReq(req);
    if (!self) return ESP_FAIL;

    // For now, simple placeholder UI
    const char* html =
        "<!DOCTYPE html><html><body>"
        "<h1>Provisioning</h1>"
        "<form method='POST' action='/submit'>"
        "SSID: <input name='ssid'><br>"
        "Password: <input name='password' type='password'><br>"
        "<button type='submit'>Save</button>"
        "</form>"
        "</body></html>";

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t ProvisioningServer::handle_scan_get(httpd_req_t* req)
{
    ProvisioningServer* self = fromReq(req);
    if (!self) return ESP_FAIL;

    // Simple synchronous scan (can be refined later)
    wifi_scan_config_t scan_cfg = {};
    esp_err_t err = esp_wifi_scan_start(&scan_cfg, true);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "scan_start failed: %d", err);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "scan failed");
        return ESP_OK;
    }

    uint16_t ap_num = 0;
    esp_wifi_scan_get_ap_num(&ap_num);

    std::vector<wifi_ap_record_t> aps;
    aps.resize(ap_num);
    if (ap_num > 0) {
        esp_wifi_scan_get_ap_records(&ap_num, aps.data());
    }

    // Very simple JSON-ish response (no dynamic allocation beyond vector)
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr_chunk(req, "[");
    for (uint16_t i = 0; i < ap_num; ++i) {
        char buf[128];
        std::snprintf(buf, sizeof(buf),
                      "%s{\"ssid\":\"%s\",\"rssi\":%d}",
                      (i == 0 ? "" : ","),
                      reinterpret_cast<const char*>(aps[i].ssid),
                      aps[i].rssi);
        httpd_resp_sendstr_chunk(req, buf);
    }
    httpd_resp_sendstr_chunk(req, "]");
    httpd_resp_sendstr_chunk(req, nullptr); // end chunked response

    return ESP_OK;
}

esp_err_t ProvisioningServer::handle_submit_post(httpd_req_t* req)
{
    ProvisioningServer* self = fromReq(req);
    if (!self) return ESP_FAIL;

    // Read body into fixed buffer (no heap)
    char buf[256];
    int total = httpd_req_recv(req, buf, sizeof(buf) - 1);
    if (total <= 0) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "no data");
        return ESP_OK;
    }
    buf[total] = '\0';

    // Very simple form parsing: ssid=...&password=...
    std::string body(buf);
    std::string ssid;
    std::string password;

    auto findField = [](const std::string& src,
                        const std::string& key,
                        std::string& out) -> bool {
        std::size_t pos = src.find(key + "=");
        if (pos == std::string::npos) return false;
        pos += key.size() + 1;
        std::size_t end = src.find("&", pos);
        if (end == std::string::npos) end = src.size();
        out = src.substr(pos, end - pos);
        return true;
    };

    bool ok_ssid = findField(body, "ssid", ssid);
    bool ok_pwd  = findField(body, "password", password);

    if (!ok_ssid || !ok_pwd) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "invalid form");
        return ESP_OK;
    }

    self->handleSubmit(ssid, password);

    httpd_resp_set_status(req, "204 No Content");
    httpd_resp_send(req, nullptr, 0);
    return ESP_OK;
}

esp_err_t ProvisioningServer::handle_status_get(httpd_req_t* req)
{
    ProvisioningServer* self = fromReq(req);
    if (!self) return ESP_FAIL;

    httpd_resp_set_type(req, "application/json");
    char buf[128];
    std::snprintf(buf, sizeof(buf),
                  "{\"state\":\"%s\"}",
                  toString(self->state()));
    httpd_resp_send(req, buf, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// --- Credential handling ---

void ProvisioningServer::handleSubmit(const std::string& ssid,
                                      const std::string& password)
{
    ESP_LOGI(TAG, "Received credentials for SSID: %s", ssid.c_str());

    // Still in provisioning phase
    if (current != ProvisioningState::Provisioning) {
        ESP_LOGW(TAG, "Ignoring credentials: not in Provisioning state");
        return;
    }

    // Basic validation
    if (ssid.empty() || password.empty()) {
        ESP_LOGW(TAG, "Invalid credentials");
        // We do NOT have a 'Failed' state anymore
        // Provisioning simply remains active until user retries
        return;
    }

    // Save credentials
    credential_store::WiFiCredential cred;
    cred.ssid     = ssid;
    cred.password = password;

    std::vector<credential_store::WiFiCredential> all;
    all.reserve(1);
    all.push_back(cred);

    bool ok = store_->saveAll(all);
    if (!ok) {
        ESP_LOGE(TAG, "Failed to save credentials");
        // Again: no Failed state in your new model
        return;
    }

    // Provisioning is complete
    setState(ProvisioningState::ProvisioningComplete);

    if (complete) complete();
}

} // namespace provisioning