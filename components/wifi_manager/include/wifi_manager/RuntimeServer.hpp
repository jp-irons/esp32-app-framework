#pragma once

#include "credential_store/CredentialApiHandler.hpp"
#include "http/HttpServer.hpp"
#include "static_assets/StaticFileHandler.hpp"
#include "wifi_manager/WiFiApiHandler.hpp"

namespace wifi_manager {

struct WiFiContext;

class RuntimeServer : public http::HttpHandler  {
  public:
    explicit RuntimeServer(WiFiContext &ctx, WiFiApiHandler &wifiApi,
                           credential_store::CredentialApiHandler &credentialApi);
    ~RuntimeServer();

    bool start(); // start HTTP server
    void stop(); // stop HTTP server

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