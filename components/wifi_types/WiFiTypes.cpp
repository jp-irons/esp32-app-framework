#include "../wifi_types/include/wifi_types/WiFiTypes.hpp"

namespace wifi_types {

const char *toString(WiFiState state) {
    switch (state) {
        case WiFiState::UNINITIALISED:
            return "Un-initialised";
        case WiFiState::STARTING:
            return "Starting";

        // Provisioning path
        case WiFiState::UNPROVISIONED_AP:
            return "Unprovisioned AP";

        case WiFiState::PROVISIONING:
            return "Provisioning";
        case WiFiState::PROVISIONING_TEST_STA:
            return "Provisioning Test STA";

        // Runtime path
        case WiFiState::STA_CONNECTING:
            return "STA Connecting";
        case WiFiState::STA_CONNECTED:
            return "STA Connected";
        case WiFiState::GOT_IP:
            return "Got IP";

        // Error / fallback
        case WiFiState::STA_DISCONNECTED:
            return "STA Disconnected";
        case WiFiState::STA_CONNECT_FAILED:
            return "STA Connect Failed";
        case WiFiState::FALLBACK_AP:
            return "Fall back AP";

        // Terminal / maintenance
        case WiFiState::STOPPING:
            return "Stopping";
        case WiFiState::STOPPED:
            return "Stopped";
    }
    return "Unknown";
}

const char *toString(WiFiAuthMode auth) {
    switch (auth) {
        case WiFiAuthMode::Open:
            return "Open";
        case WiFiAuthMode::Unknown:
            return "Unknown";
        case WiFiAuthMode::WEP:
            return "WEP";
        case WiFiAuthMode::WPA_PSK:
            return "WPA_PSK";
        case WiFiAuthMode::WPA2_PSK:
            return "WPA2_PSK";
        case WiFiAuthMode::WPA3_PSK:
            return "WPA3_PSK";
        case WiFiAuthMode::WPA_WPA2_PSK:
            return "WPA_WPA2_PSK";
    }
    return "UNKNOWN";
}

const char *toString(WiFiError err) {
    switch (err) {
        case WiFiError::NONE:
            return "NONE";

        // STA failures
        case WiFiError::AUTH_FAILED:
            return "AUTH_FAILED";
        case WiFiError::NO_AP_FOUND:
            return "NO_AP_FOUND";
        case WiFiError::CONNECTION_TIMEOUT:
            return "CONNECTION_TIMEOUT";
        case WiFiError::HANDSHAKE_TIMEOUT:
            return "HANDSHAKE_TIMEOUT";

        // Provisioning failures
        case WiFiError::INVALID_CREDENTIALS:
            return "INVALID_CREDENTIALS";
        case WiFiError::PROVISIONING_TIMEOUT:
            return "PROVISIONING_TIMEOUT";

        // Driver/system failures
        case WiFiError::DRIVER_INIT_FAILED:
            return "DRIVER_INIT_FAILED";
        case WiFiError::DRIVER_START_FAILED:
            return "DRIVER_START_FAILED";
        case WiFiError::UNKNOWN:
            return "UNKNOWN";
    }
    return "UNKNOWN";
}

} // namespace wifi_manager