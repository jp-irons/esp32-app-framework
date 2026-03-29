#pragma once

#include "WiFiContext.hpp"
#include "esp_event_base.h"


namespace wifi_manager {

class WiFiManager {
public:
    explicit WiFiManager(WiFiContext* ctx);

    void start();
    void tick();

    WiFiState state() const { return ctx->state; }

	void loop(); // no logic to go in loop. Heartbeat, watchdog etc. ok

private:
    WiFiContext* ctx;

    // The authoritative state transition function
    void transitionTo(WiFiState s);

	void handleUnprovisionedAP();
	void handleProvisioningStaTest();
	void handleProvisioningFailed();
	void handleRuntimeSta();
	void tryNextCredential();

    // WiFi event handlers
    static void wifiEventHandler(void* arg,
                                 esp_event_base_t base,
                                 int32_t id,
                                 void* data);

    static void ipEventHandler(void* arg,
                               esp_event_base_t base,
                               int32_t id,
                               void* data);

};

WiFiManager* create(WiFiContext& ctx);

} // namespace wifi_manager