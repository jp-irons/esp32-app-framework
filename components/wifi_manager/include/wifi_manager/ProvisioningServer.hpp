#pragma once

#include "common/Result.hpp"
#include "credential_store/CredentialApiHandler.hpp"
#include "http/HttpHandler.hpp"
#include "http/HttpServer.hpp"
#include "static_assets/StaticFileHandler.hpp"
#include "wifi_manager/WiFiApiHandler.hpp"

namespace http {
class HttpRequest;
class HttpResponse;
} // namespace http

namespace wifi_manager {

struct WiFiContext;

class ProvisioningServer : public http::HttpHandler {
  public:
    explicit ProvisioningServer(WiFiContext &ctx
		, WiFiApiHandler &wifiApi
		, credential_store::CredentialApiHandler &credentialApi);
    ~ProvisioningServer();

    bool start();
    void stop();

    common::Result handle(http::HttpRequest &req, http::HttpResponse &res) override;

  private:
    WiFiContext &ctx;

    http::HttpServer server;
    static_assets::StaticFileHandler staticHandler;
    static_assets::StaticFileHandler fallbackHandler;
    wifi_manager::WiFiApiHandler wifiHandler;
    credential_store::CredentialApiHandler credentialHandler;

    bool routesRegistered = false;
	
};

} // namespace wifi_manager