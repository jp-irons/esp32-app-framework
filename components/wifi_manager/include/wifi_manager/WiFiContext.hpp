#pragma once

#include "wifi_types/WiFiTypes.hpp"
namespace credential_store {
class CredentialStore;
}

namespace wifi_types {
enum class WiFiState;
}

namespace wifi_manager {

class WiFiInterface;
class WiFiStateMachine;
class ProvisioningServer;
class RuntimeServer;

struct WiFiContext {
    ProvisioningServer *provisioningServer = nullptr;
    RuntimeServer *runtimeServer = nullptr;
    credential_store::CredentialStore *credentialStore = nullptr;

    WiFiInterface *wifiInterface = nullptr;
    WiFiStateMachine *stateMachine = nullptr;

    wifi_types::ApConfig apConfig;
    wifi_types::WiFiCredential currentWiFiCred;
	
	std::string rootUri;

};

} // namespace wifi_manager
