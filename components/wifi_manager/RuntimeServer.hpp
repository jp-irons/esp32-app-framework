#pragma once

#include "WiFiContext.hpp"
#include "esp_http_server.h"

namespace wifi_manager {

class RuntimeServer {
public:
	explicit RuntimeServer(WiFiContext* ctx);

    void start();
    void stop();

    void handleStatusRequest();

private:
	WiFiContext* ctx;
	httpd_handle_t server = nullptr;
	
	bool registerHandlers();
};

} // namespace wifi_manager
