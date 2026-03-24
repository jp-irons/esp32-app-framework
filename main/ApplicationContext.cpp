#include "ApplicationContext.hpp"
#include "WiFiManager.hpp"
#include "RuntimeServer.hpp"
#include "ProvisioningServer.hpp"
#include <esp_log.h>

using wifi_manager::WiFiManager;

static const char* TAG = "ApplicationServer";

ApplicationContext::ApplicationContext()
    : credentialStore()
{
    ESP_LOGI("ApplicationServer", "constructing");
    runtimeServer      = std::make_unique<RuntimeServer>(*this);
    provisioningServer = std::make_unique<ProvisioningServer>(*this);
    wifiManager        = std::make_unique<wifi_manager::WiFiManager>(*this);
}

ApplicationContext::~ApplicationContext() = default;

void ApplicationContext::start()
{
    ESP_LOGI(TAG, "starting");
    credentialStore.begin();      // opens NVS namespace
    wifiManager->begin();          // starts Wi-Fi
}
