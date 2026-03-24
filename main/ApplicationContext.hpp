#pragma once

#include <memory>
#include "CredentialStore.hpp"
#include "EmbeddedFiles.hpp"

class ProvisioningServer;
class RuntimeServer;

namespace wifi_manager {
    class WiFiManager;
}

class ApplicationContext {
public:
    ApplicationContext();
    ~ApplicationContext();   // declaration ONLY

    CredentialStore credentialStore;
    std::unique_ptr<wifi_manager::WiFiManager> wifiManager;
    std::unique_ptr<RuntimeServer> runtimeServer;
    std::unique_ptr<ProvisioningServer> provisioningServer;

    void start();
};