#pragma once

#include "WiFiTypes.hpp"
#include "credential_store/CredentialStore.hpp"
#include "esp_event_base.h"
#include "esp_netif_types.h"
#include "esp_wifi_types_generic.h"

namespace common {
enum class Result;
}

namespace wifi_manager {

struct WiFiContext;
// forward declaration
class WiFiStateMachine;
// forward declaration

class WiFiInterface {
  public:
    explicit WiFiInterface(WiFiContext &ctx);

    void startDriver();
    void stopDriver();

    void startAp(const ApConfig &cfg);
    void stopAp();

    void connectSta(const StaConfig &cfg);
    void disconnectSta();

    void startProvisioningServer();
    void stopProvisioningServer();

    void startRuntimeServer();
    void stopRuntimeServer();

	common::Result scan(std::vector<WiFiAp>& results);

  private:
    WiFiContext &ctx;

    esp_netif_t *apNetif = nullptr;
    esp_netif_t *staNetif = nullptr;
	
	bool driverStarted = false;

	bool apActive = false;
	bool staActive = false;

	wifi_mode_t currentMode = WIFI_MODE_NULL;

    static void wifiEventHandler(void *arg, esp_event_base_t base, int32_t id, void *data);

    static void ipEventHandler(void *arg, esp_event_base_t base, int32_t id, void *data);

    void handleWiFiEvent(esp_event_base_t base, int32_t id, void *data);

    void handleIPEvent(esp_event_base_t base, int32_t id, void *data);

    void connectTo(const credential_store::WiFiCredential &cred);

    void onSTAConnected();
    void onSTADisconnected(uint8_t reason);
	static WiFiAuthMode toAuthMode(wifi_auth_mode_t mode);
	wifi_mode_t computeMode() const;
	bool setStaState(bool enable);


};

} // namespace wifi_manager
