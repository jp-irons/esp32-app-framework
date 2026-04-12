#pragma once

#include "credential_store/CredentialApiHandler.hpp"
#include "credential_store/CredentialStore.hpp"
#include "wifi_manager/WiFiApiHandler.hpp"
#include "wifi_manager/WiFiContext.hpp"
//#include "wifi_manager/WiFiApiHandler.hpp"

namespace credential_store {
class CredentialStore;
}

namespace framework {

class FrameworkContext {
  public:
    explicit FrameworkContext(const wifi_manager::ApConfig &provisioningApConfig,
                              std::string rootUri = "/framework/api");

    ~FrameworkContext();

    const std::string &getRootUri() const {
        return rootUri_;
    }
    credential_store::CredentialStore &getCredentialStore() const;
    wifi_manager::WiFiContext &getWiFiContext() const;

    void start();
    void stop();

  private:
  std::string rootUri_;
    // Always-present value types
    wifi_manager::WiFiContext wifiCtx;
    credential_store::CredentialStore credentialStore;

    // Owned components
    wifi_manager::ProvisioningServer *provisioningServer = nullptr;
    wifi_manager::RuntimeServer *runtimeServer = nullptr;
    wifi_manager::WiFiInterface *wifiInterface = nullptr;
    wifi_manager::WiFiStateMachine *wifiStateMachine = nullptr;

    // API handlers
    wifi_manager::WiFiApiHandler *wifiApi = nullptr;
    credential_store::CredentialApiHandler *credentialApi = nullptr;
};

} // namespace framework