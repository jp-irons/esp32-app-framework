#include "ApplicationContext.hpp"
#include <esp_log.h>

static const char* TAG = "ApplicationServer";

ApplicationContext::ApplicationContext()
    : credentialStore()
{
    ESP_LOGI(TAG, "constructing");
    runtimeServer = std::make_unique<RuntimeServer>(*this);
    provisioningServer = std::make_unique<ProvisioningServer>(*this);
    wifiManager = std::make_unique<WiFiManager>(*this);
}

void ApplicationContext::start()
{
    ESP_LOGI(TAG, "starting");
    credentialStore.begin();      // opens NVS namespace
    wifiManager->begin();          // starts Wi-Fi
}
