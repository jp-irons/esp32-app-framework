#pragma once

#include <vector>
#include <string>
#include <stdint.h>
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_timer.h"

// Forward declarations
class CredentialStore;
class WifiScanner;
class ProvisioningServer;
class RuntimeServer;
class ApplicationContext;

enum class WiFiState;
struct WiFiEntry;


// State machine
enum class WiFiState {
    UNPROVISIONED_AP,
    PROVISIONING_STA_TEST,
    PROVISIONING_FAILED,
    RUNTIME_STA
};

// Multi‑SSID entry
struct WiFiEntry {
    std::string ssid;
    std::string password;
    int priority;
    uint8_t bssid[6];
    bool bssidLocked;
};

struct ScanResult {
    std::string ssid;
    uint8_t bssid[6];
    int rssi;
    wifi_auth_mode_t authmode;
    int channel;
};


class WiFiManager {
public:
    WiFiManager(ApplicationContext& ctx);

    void begin();
    void setEntries(const std::vector<WiFiEntry>& newEntries);

    const std::vector<WiFiEntry>& getEntries() const { return entries; }
    WiFiState getState() const { return state; }
    std::string stateToString(WiFiState s) const;
    bool isConnected() const { return staConnected; }
    const std::string& getCurrentSSID() const { return currentSSID; }
    std::string getLastAttemptedSsid() const;
   int getLastErrorReason() const { return lastErrorReason; }

    static void wifiEventHandler(void* arg, esp_event_base_t base, int32_t id, void* data);
    static void ipEventHandler(void* arg, esp_event_base_t base, int32_t id, void* data);
    bool scan(std::vector<ScanResult>& out);

private:
    void handleWifiEvent(esp_event_base_t base, int32_t id, void* data);
    void handleIpEvent(esp_event_base_t base, int32_t id, void* data);

    void enterState(WiFiState newState);
    void startAPForProvisioning();
    void stopAP();
    void startRuntimeServer();
    void stopRuntimeServer();


    void tryNextEntry();
    void configureSTAForEntry(const WiFiEntry& entry);
    void startSTAConnectAttempt();
    void handleConnectFailure(int reason);

    void startConnectTimer();
    void stopConnectTimer();
    static void connectTimeoutCallback(void* arg);

private:
    ApplicationContext& ctx;

    WiFiState state = WiFiState::UNPROVISIONED_AP;

    std::vector<WiFiEntry> entries;
    int currentIndex = -1;

    bool staConnected = false;
    bool hasEverConnected = false;

    std::string currentSSID;
    std::string lastAttemptedSsid;

    int lastErrorReason = 0;

    esp_timer_handle_t connectTimer = nullptr;
};