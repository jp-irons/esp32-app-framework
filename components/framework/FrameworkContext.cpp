#include "framework/FrameworkContext.hpp"

#include "credential_store/CredentialApiHandler.hpp"
#include "credential_store/CredentialStore.hpp"
#include "esp_event.h"
#include "esp_netif.h"
#include "logger/Logger.hpp"
#include "nvs_flash.h"
#include "wifi_manager/ProvisioningServer.hpp"
#include "wifi_manager/RuntimeServer.hpp"
#include "wifi_manager/WiFiApiHandler.hpp"
#include "wifi_manager/WiFiInterface.hpp"
#include "wifi_manager/WiFiStateMachine.hpp"

namespace framework {

static logger::Logger log{"FrameworkContext"};

FrameworkContext::FrameworkContext(
	const wifi_manager::ApConfig& apConfig, 
	std::string rootUri)
    : rootUri_(std::move(rootUri)) {
    log.debug("constructor");

    // 1. Initialize NVS
    log.debug("nvs_flash_init");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        log.error("flash erase then init");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    // 2. Initialize event loop
    log.debug("init event loop");
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // 3. Initialize netif
    log.debug("init netif");
    ESP_ERROR_CHECK(esp_netif_init());

    log.debug("AP SSID %s", apConfig.ssid.c_str());
    wifiCtx.apConfig = apConfig;
    wifiCtx.credentialStore = &credentialStore;
	wifiCtx.rootUri = rootUri_;
    wifiApi = new wifi_manager::WiFiApiHandler(wifiCtx);
    credentialApi = new credential_store::CredentialApiHandler(credentialStore);

    // 2. Create servers (AP + Runtime HTTP)
    provisioningServer = new wifi_manager::ProvisioningServer(wifiCtx, *wifiApi, *credentialApi);

    runtimeServer = new wifi_manager::RuntimeServer(wifiCtx);
    wifiCtx.provisioningServer = provisioningServer;
    wifiCtx.runtimeServer = runtimeServer;

    // 3. Create WiFiInterface
    wifiInterface = new wifi_manager::WiFiInterface(wifiCtx);
    wifiCtx.wifiInterface = wifiInterface;

    // 4. Create WiFiStateMachine
    wifiStateMachine = new wifi_manager::WiFiStateMachine(wifiCtx);
    wifiCtx.stateMachine = wifiStateMachine;

    // 5. Create API handlers
    wifiApi = new wifi_manager::WiFiApiHandler(wifiCtx);
    credentialApi = new credential_store::CredentialApiHandler(credentialStore);
}

FrameworkContext::~FrameworkContext() {
    stop();
    delete provisioningServer;
    delete runtimeServer;
    delete wifiInterface;
    delete wifiStateMachine;
    delete wifiApi;
    delete credentialApi;
}

void FrameworkContext::start() {
    log.debug("start");
    wifiStateMachine->start();
}

void FrameworkContext::stop() {}

} // namespace framework