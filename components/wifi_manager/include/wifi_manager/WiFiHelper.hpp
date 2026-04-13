#pragma once

#include "esp_wifi_types_generic.h"
#include "wifi_types/WiFiTypes.hpp"

namespace wifi_manager {

wifi_auth_mode_t toEspAuth(wifi_types::WiFiAuthMode mode);

wifi_config_t toEspConfig(const wifi_types::WiFiCredential& cred);

wifi_config_t makeStaConfig(const wifi_types::WiFiCredential& cred);

wifi_config_t makeApConfig(const wifi_types::ApConfig& cfg);

wifi_types::WiFiError toWiFiError(uint8_t reason);

std::string toString(uint8_t reason);

} // namespace wifi_manager
