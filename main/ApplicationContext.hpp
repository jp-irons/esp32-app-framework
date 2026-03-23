#pragma once

#include <memory>
#include "CredentialStore.hpp"
#include "WiFiManager.hpp"
#include "EmbeddedFiles.hpp"
#include "RuntimeServer.hpp"
#include "ProvisioningServer.hpp"

class ApplicationContext {
public:
    CredentialStore credentialStore;
    std::unique_ptr<WiFiManager> wifiManager;

    std::unique_ptr<RuntimeServer> runtimeServer;
    std::unique_ptr<ProvisioningServer> provisioningServer;

    ApplicationContext();
    void start();
};