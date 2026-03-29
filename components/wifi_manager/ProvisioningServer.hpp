#pragma once

#include <functional>
#include <string>
#include "ProvisioningState.hpp"
#include "WiFiContext.hpp"
#include "esp_http_server.h"

using namespace wifi_manager;

namespace credential_store {
	class CredentialStore;
}

namespace wifi_manager {
	
class ProvisioningServer {
public:
	explicit ProvisioningServer(WiFiContext* ctx);

    using CompletionCallback = std::function<void()>;
    using FailureCallback    = std::function<void()>;

    ProvisioningServer(credential_store::CredentialStore* store);

    // Explicit lifecycle
    bool start();   // start AP + HTTP server
    void stop();    // stop HTTP server + AP

    ProvisioningState state() const { return current; }

    void onComplete(CompletionCallback cb) { complete = cb; }
    void onFailure(FailureCallback cb)     { failure  = cb; }

	// Static HTTP handlers (C-style, forward to instance)
	static esp_err_t handle_root_get(httpd_req_t* req);
	static esp_err_t handle_scan_get(httpd_req_t* req);
	static esp_err_t handle_submit_post(httpd_req_t* req);
	static esp_err_t handle_status_get(httpd_req_t* req);
	credential_store::CredentialStore* store() const { return store_; }

private:
	WiFiContext* ctx;
	credential_store::CredentialStore* store_;
	ProvisioningState current;
	
	CompletionCallback complete;
	FailureCallback    failure;
	
	httpd_handle_t server;
	
	bool registerHandlers();

	bool startAP();
	void stopAP();
	bool startHttp();
	void stopHttp();
	
	void setState(ProvisioningState s);
	
	void handleSubmit(const std::string& ssid,
	                  const std::string& password);
	
	static ProvisioningServer* fromReq(httpd_req_t* req);
	
	// Pattern 2: static handler structs INSIDE the class
	static httpd_uri_t uri_root;
	static httpd_uri_t uri_scan;
	static httpd_uri_t uri_submit;
	static httpd_uri_t uri_status;
	
};

} // namespace provisioning
