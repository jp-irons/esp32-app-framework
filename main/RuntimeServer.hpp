#pragma once
#include "esp_http_server.h"
#include "ApplicationContext.hpp"
#include "CredentialApiHandler.hpp"
#include "WiFiApiHandler.hpp"

// forward declarations
class CredentialApiHandler;
class WiFiApiHandler;
class ProvisioningApiHandler;
esp_err_t serveEmbedded(httpd_req_t* req, const char* path);

class RuntimeServer {
public:
    explicit RuntimeServer(ApplicationContext& ctx);

    esp_err_t start();
    esp_err_t stop();

private:
    ApplicationContext& ctx;
    httpd_handle_t server = nullptr;

    std::unique_ptr<CredentialApiHandler> credentialApi;
    std::unique_ptr<WiFiApiHandler> wifiApi;
    std::unique_ptr<ProvisioningApiHandler> provisioningApi;

    static esp_err_t dispatchApi(httpd_req_t* req);
};
