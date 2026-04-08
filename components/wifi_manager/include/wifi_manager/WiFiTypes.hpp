#pragma once

#include "esp_wifi_types_generic.h"
#include <cstring>
#include <string>

namespace wifi_manager {

enum class WiFiAuthMode { 
	Open, WEP, WPA_PSK, WPA2_PSK, WPA_WPA2_PSK, WPA3_PSK, Unknown 
};

struct WiFiAp {
    std::string ssid; // Human-readable SSID
    uint8_t bssid[6]; // MAC address of the AP
    int rssi; // Signal strength (dBm)
    WiFiAuthMode auth; // Security type
    int channel; // Primary channel
};

struct ApConfig {
    std::string ssid;
    std::string password; // empty = open AP
    uint8_t channel = 1;
    uint8_t maxConnections = 4;
    bool hidden = false;
	WiFiAuthMode auth = WiFiAuthMode::WPA2_PSK;
	wifi_config_t toEspConfig() const {
	    wifi_config_t cfg = {};
	    auto& ap = cfg.ap;

	    strncpy((char*)ap.ssid, ssid.c_str(), sizeof(ap.ssid));
	    strncpy((char*)ap.password, password.c_str(), sizeof(ap.password));

	    ap.ssid_len = ssid.length();
	    ap.channel = channel;
	    ap.max_connection = maxConnections;
	    ap.authmode = toEspAuth(auth);

	    return cfg;
	}
	
	static wifi_auth_mode_t toEspAuth(WiFiAuthMode mode) {
	    switch (mode) {
	        case WiFiAuthMode::Open:         return WIFI_AUTH_OPEN;
	        case WiFiAuthMode::WEP:          return WIFI_AUTH_WEP;
	        case WiFiAuthMode::WPA_PSK:      return WIFI_AUTH_WPA_PSK;
	        case WiFiAuthMode::WPA2_PSK:     return WIFI_AUTH_WPA2_PSK;
	        case WiFiAuthMode::WPA_WPA2_PSK: return WIFI_AUTH_WPA_WPA2_PSK;
	        case WiFiAuthMode::WPA3_PSK:     return WIFI_AUTH_WPA3_PSK;
	        default:                         return WIFI_AUTH_OPEN;
	    }
	}

};

struct StaConfig {
    std::string ssid;
    std::string password;
	wifi_config_t toEspConfig() const {
	    wifi_config_t cfg = {};
	    auto& sta = cfg.sta;

	    strncpy((char*)sta.ssid, ssid.c_str(), sizeof(sta.ssid));
	    strncpy((char*)sta.password, password.c_str(), sizeof(sta.password));

	    sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
	    sta.pmf_cfg.capable = true;
	    sta.pmf_cfg.required = false;

	    return cfg;
	}
};

enum class WiFiState {
    UNINITIALISED, // Before WiFiInterface::start()
    STARTING, // WiFi driver is being initialised

    // Provisioning path
    UNPROVISIONED_AP, // SoftAP running for provisioning
    PROVISIONING, // Handling provisioning requests
    PROVISIONING_TEST_STA, // Testing STA credentials

    // Runtime path
    STA_CONNECTING, // Attempting STA connection
    STA_CONNECTED, // Connected, waiting for IP
    GOT_IP, // Fully online

    // Error / fallback
    STA_DISCONNECTED, // Lost connection
    STA_CONNECT_FAILED, // Credentials invalid or AP unreachable
    FALLBACK_AP, // Returning to AP mode after failure

    // Terminal / maintenance
    STOPPING,
    STOPPED
};

enum class WiFiError {
    NONE,

    // STA failures
    AUTH_FAILED,
    NO_AP_FOUND,
    CONNECTION_TIMEOUT,
    HANDSHAKE_TIMEOUT,

    // Provisioning failures
    INVALID_CREDENTIALS,
    PROVISIONING_TIMEOUT,

    // Driver/system failures
    DRIVER_INIT_FAILED,
    DRIVER_START_FAILED,
    UNKNOWN
};

const char *toString(WiFiState state);

const char *toString(WiFiError err);

const char *toString(WiFiAuthMode auth);

WiFiError toWiFiError(uint8_t reason);

} // namespace wifi_manager