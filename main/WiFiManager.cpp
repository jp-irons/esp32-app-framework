#include "WiFiManager.hpp"
#include "ApplicationContext.hpp"

#include "esp_log.h"
#include "esp_netif.h"
#include "esp_timer.h"
#include <cstring>
#include <algorithm>

static const char* TAG = "WiFiManager";

WiFiManager::WiFiManager(ApplicationContext& ctx)
    : ctx(ctx)
{
}

void WiFiManager::begin()
{
    ESP_LOGI(TAG, "Initialising");

    // Create default netifs
    esp_netif_create_default_wifi_ap();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Register event handlers
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &WiFiManager::wifiEventHandler, this, nullptr));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &WiFiManager::ipEventHandler, this, nullptr));

    ESP_ERROR_CHECK(esp_wifi_start());

    // Load saved credentials + state
    WiFiState savedState;
    bool haveEntries = ctx.credentialStore.loadEntries(entries);
    bool haveState = ctx.credentialStore.loadState(savedState);
    (void)haveState;

    if (!haveEntries) {
        enterState(WiFiState::UNPROVISIONED_AP);
    } else {
        enterState(WiFiState::PROVISIONING_STA_TEST);
    }
}

void WiFiManager::setEntries(const std::vector<WiFiEntry>& newEntries)
{
    entries = newEntries;

    // Sort by priority
    std::sort(entries.begin(), entries.end(),
              [](const WiFiEntry& a, const WiFiEntry& b) {
                  return a.priority < b.priority;
              });

    ctx.credentialStore.saveEntries(entries);

    currentIndex = -1;
    lastErrorReason = 0;

    enterState(WiFiState::PROVISIONING_STA_TEST);
}

void WiFiManager::enterState(WiFiState newState)
{
    ESP_LOGI(TAG, "enterState: %s → %s",
         stateToString(state).c_str(),
         stateToString(newState).c_str());

    state = newState;
    ctx.credentialStore.saveState(state);

    switch (state) {

    case WiFiState::UNPROVISIONED_AP:
        ESP_LOGI(TAG, "State: UNPROVISIONED_AP");
        ctx.runtimeServer->stop();
        startAPForProvisioning();
        ctx.provisioningServer->start();
        break;

    case WiFiState::PROVISIONING_STA_TEST:
        ESP_LOGI(TAG, "State: PROVISIONING_STA_TEST");
        ctx.runtimeServer->stop();
        stopAP();
        ctx.provisioningServer->stop();
        currentIndex = -1;
        tryNextEntry();
        break;

    case WiFiState::PROVISIONING_FAILED:
        ESP_LOGW(TAG, "State: PROVISIONING_FAILED");
        ctx.runtimeServer->stop();
        startAPForProvisioning();
        ctx.provisioningServer->start();
        break;

    case WiFiState::RUNTIME_STA:
        ESP_LOGI(TAG, "State: RUNTIME_STA");
        stopAP();
        ctx.provisioningServer->stop();
        startRuntimeServer();
        break;
    }
}

void WiFiManager::tryNextEntry()
{
    currentIndex++;

    if (currentIndex >= entries.size()) {
        ESP_LOGW(TAG, "All SSIDs failed");
        enterState(WiFiState::PROVISIONING_FAILED);
        return;
    }

    const WiFiEntry& entry = entries[currentIndex];
    ESP_LOGI(TAG, "Trying SSID %s (priority %d)", entry.ssid.c_str(), entry.priority);

    configureSTAForEntry(entry);
    startSTAConnectAttempt();
}

void WiFiManager::configureSTAForEntry(const WiFiEntry& entry)
{
    wifi_config_t cfg = {};
    strncpy((char*)cfg.sta.ssid, entry.ssid.c_str(), sizeof(cfg.sta.ssid));
    strncpy((char*)cfg.sta.password, entry.password.c_str(), sizeof(cfg.sta.password));

    if (entry.bssidLocked) {
        cfg.sta.bssid_set = 1;
        memcpy(cfg.sta.bssid, entry.bssid, 6);
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &cfg));

    currentSSID = entry.ssid;
}

void WiFiManager::startSTAConnectAttempt()
{
    ESP_LOGI(TAG, "Connecting to %s", currentSSID.c_str());
    lastAttemptedSsid = currentSSID;
    esp_wifi_connect();
    startConnectTimer();
}

void WiFiManager::startAPForProvisioning()
{
    wifi_config_t ap = {};
    strcpy((char*)ap.ap.ssid, "ESP32-Provision");
    ap.ap.ssid_len       = strlen("ESP32-Provision");
    ap.ap.channel        = 1;
    ap.ap.max_connection = 4;
    ap.ap.authmode       = WIFI_AUTH_OPEN;
    ap.ap.ssid_hidden    = 0;   // important on ESP-IDF 5.x

    // 1. Stop Wi-Fi cleanly
    ESP_ERROR_CHECK(esp_wifi_stop());

    // 2. Apply AP mode + config AFTER stop
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap));

    // 3. Start Wi-Fi with the new AP config
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Started AP");
}


void WiFiManager::stopAP()
{
    wifi_mode_t mode;
    esp_wifi_get_mode(&mode);

    if (mode == WIFI_MODE_AP || mode == WIFI_MODE_APSTA) {
        ESP_ERROR_CHECK(esp_wifi_stop());
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_start());
        ESP_LOGI(TAG, "Stopped AP");
    }
}

void WiFiManager::startRuntimeServer()
{
    ctx.runtimeServer->start();
}

void WiFiManager::stopRuntimeServer()
{
    ctx.runtimeServer->stop();
}

void WiFiManager::wifiEventHandler(void* arg, esp_event_base_t base, int32_t id, void* data)
{
    static_cast<WiFiManager*>(arg)->handleWifiEvent(base, id, data);
}

void WiFiManager::ipEventHandler(void* arg, esp_event_base_t base, int32_t id, void* data)
{
    static_cast<WiFiManager*>(arg)->handleIpEvent(base, id, data);
}

void WiFiManager::handleWifiEvent(esp_event_base_t base, int32_t id, void* data)
{
    if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED) {

        wifi_event_sta_disconnected_t* disc = (wifi_event_sta_disconnected_t*)data;
        lastErrorReason = disc->reason;

        ESP_LOGW(TAG, "Disconnected: reason=%d", disc->reason);

        stopConnectTimer();

        if (state == WiFiState::PROVISIONING_STA_TEST) {
            handleConnectFailure(disc->reason);
            return;
        }

        if (state == WiFiState::RUNTIME_STA) {
            ESP_LOGW(TAG, "Runtime disconnect → retrying same SSID");
            startSTAConnectAttempt();
            return;
        }
    }
}

void WiFiManager::handleIpEvent(esp_event_base_t base, int32_t id, void* data)
{
    if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {

        stopConnectTimer();

        staConnected = true;
        hasEverConnected = true;
        lastErrorReason = 0;

        ip_event_got_ip_t* event = (ip_event_got_ip_t*)data;

        ESP_LOGI(TAG, "Connected to %s, IP: " IPSTR,
                 currentSSID.c_str(), IP2STR(&event->ip_info.ip));

        enterState(WiFiState::RUNTIME_STA);
    }
}

void WiFiManager::handleConnectFailure(int reason)
{
    ESP_LOGW(TAG, "Connect failed for %s (reason=%d)", currentSSID.c_str(), reason);

    switch (reason) {
    case WIFI_REASON_AUTH_FAIL:
    case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
    case WIFI_REASON_HANDSHAKE_TIMEOUT:
        ESP_LOGE(TAG, "Auth failure → immediate fail");
        tryNextEntry();
        return;

    case WIFI_REASON_NO_AP_FOUND:
    case WIFI_REASON_BEACON_TIMEOUT:
        ESP_LOGW(TAG, "AP not found or weak signal → next SSID");
        tryNextEntry();
        return;

    default:
        ESP_LOGW(TAG, "Transient failure → retry once");
        startSTAConnectAttempt();
        return;
    }
}

void WiFiManager::startConnectTimer()
{
    if (connectTimer) {
        esp_timer_stop(connectTimer);
        esp_timer_delete(connectTimer);
    }

    esp_timer_create_args_t args = {};
    args.callback = &WiFiManager::connectTimeoutCallback;
    args.arg = this;
    args.name = "connect_timeout";

    ESP_ERROR_CHECK(esp_timer_create(&args, &connectTimer));
    ESP_ERROR_CHECK(esp_timer_start_once(connectTimer, 10 * 1000 * 1000)); // 10s
}

void WiFiManager::stopConnectTimer()
{
    if (connectTimer) {
        esp_timer_stop(connectTimer);
        esp_timer_delete(connectTimer);
        connectTimer = nullptr;
    }
}

void WiFiManager::connectTimeoutCallback(void* arg)
{
    WiFiManager* self = static_cast<WiFiManager*>(arg);
    ESP_LOGW(TAG, "Connect timeout for %s", self->currentSSID.c_str());
    self->handleConnectFailure(-1);
}

bool WiFiManager::scan(std::vector<ScanResult>& out)
{
    // This is literally your old WifiScanner::scan implementation
    // moved here unchanged.

    // Start scan
    wifi_scan_config_t config = {};
    config.show_hidden = true;

    if (esp_wifi_scan_start(&config, true) != ESP_OK)
        return false;

    // Get results
    uint16_t count = 0;
    esp_wifi_scan_get_ap_num(&count);

    std::vector<wifi_ap_record_t> aps(count);
    if (esp_wifi_scan_get_ap_records(&count, aps.data()) != ESP_OK)
        return false;

    out.clear();
    out.reserve(count);

    for (auto& ap : aps) {
        ScanResult r;
        r.ssid     = reinterpret_cast<const char*>(ap.ssid);
        r.rssi     = ap.rssi;
        r.authmode = ap.authmode;
        r.channel  = ap.primary;
        memcpy(r.bssid, ap.bssid, 6);
        out.push_back(r);
    }
    return true;
}

std::string WiFiManager::stateToString(WiFiState s) const {
    switch (s) {
        case WiFiState::UNPROVISIONED_AP:       return "UNPROVISIONED_AP";
        case WiFiState::PROVISIONING_STA_TEST:  return "PROVISIONING_STA_TEST";
        case WiFiState::PROVISIONING_FAILED:    return "PROVISIONING_FAILED";
        case WiFiState::RUNTIME_STA:            return "RUNTIME_STA";
        default:                                return "UNKNOWN";
    }
}

std::string WiFiManager::getLastAttemptedSsid() const {
    return lastAttemptedSsid;
}

void WiFiManager::onCredentialsChanged()
{
    ESP_LOGI(TAG, "Credentials changed, scheduling WiFi restart");

    // Run state transition on a separate task
    xTaskCreate(
        [](void* arg) {
            WiFiManager* self = static_cast<WiFiManager*>(arg);
            self->enterState(WiFiState::UNPROVISIONED_AP);
            vTaskDelete(nullptr);
        },
        "wifi_restart_task",
        4096,
        this,
        5,
        nullptr
    );
}
