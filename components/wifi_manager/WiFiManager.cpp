#include "WiFiManager.hpp"
#include "ProvisioningServer.hpp"   // ← REQUIRED
#include "RuntimeServer.hpp"        // ← REQUIRED
#include "CredentialStore.hpp"

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event_base.h"


using namespace wifi_manager;

namespace wifi_manager {

static const char* TAG = "WiFiManager";

WiFiManager::WiFiManager(WiFiContext* ctx)
    : ctx(ctx)
{
}

void WiFiManager::transitionTo(WiFiState s)
{
    ESP_LOGI(TAG, "State -> %d", static_cast<int>(s));
    ctx->state = s;
}

void WiFiManager::start()
{
    transitionTo(WiFiState::UNPROVISIONED_AP);
}

void WiFiManager::tick()
{
    switch (ctx->state) {
        case WiFiState::UNPROVISIONED_AP:       handleUnprovisionedAP(); break;
        case WiFiState::PROVISIONING_STA_TEST:  handleProvisioningStaTest(); break;
        case WiFiState::PROVISIONING_FAILED:    handleProvisioningFailed(); break;
        case WiFiState::RUNTIME_STA:            handleRuntimeSta(); break;
    }
}

void WiFiManager::handleProvisioningStaTest()
{
	ctx->loadedCreds.clear();
	ctx->creds->loadAll(ctx->loadedCreds);
	ctx->currentCredIndex = 0;

	if (ctx->loadedCreds.empty()) {
	    ESP_LOGW(TAG, "No credentials → returning to AP");
	    transitionTo(WiFiState::UNPROVISIONED_AP);
	    return;
	}
	tryNextCredential();
}

void WiFiManager::tryNextCredential()
{
    if (ctx->currentCredIndex >= ctx->loadedCreds.size()) {
        ESP_LOGW(TAG, "All credentials failed");
        transitionTo(WiFiState::PROVISIONING_FAILED);
        return;
    }

    const auto& cred = ctx->loadedCreds[ctx->currentCredIndex];

    wifi_config_t cfg = {};
    strncpy((char*)cfg.sta.ssid, cred.ssid.c_str(), sizeof(cfg.sta.ssid));
    strncpy((char*)cfg.sta.password, cred.password.c_str(), sizeof(cfg.sta.password));

    ESP_LOGI(TAG, "Trying credential %zu: SSID='%s'",
             ctx->currentCredIndex, cred.ssid.c_str());

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &cfg);
    esp_wifi_start();
    esp_wifi_connect();
}



void WiFiManager::wifiEventHandler(void* arg,
                                   esp_event_base_t base,
                                   int32_t id,
                                   void* data)
{
    auto* self = static_cast<WiFiManager*>(arg);

	if (id == WIFI_EVENT_STA_DISCONNECTED) {
	    ESP_LOGW(TAG, "Credential %zu failed", self->ctx->currentCredIndex);
	    self->ctx->currentCredIndex++;
	    self->tryNextCredential();
	}
	if (id == IP_EVENT_STA_GOT_IP) {
	    ESP_LOGI(TAG, "Connected using credential %zu", self->ctx->currentCredIndex);
	    self->transitionTo(WiFiState::RUNTIME_STA);
	}


}

void WiFiManager::ipEventHandler(void* arg,
                                 esp_event_base_t base,
                                 int32_t id,
                                 void* data)
{
    auto* self = static_cast<WiFiManager*>(arg);

    if (id == IP_EVENT_STA_GOT_IP) {
        self->transitionTo(WiFiState::RUNTIME_STA);
    }
}

void WiFiManager::handleProvisioningFailed()
{
    // Go back to AP mode
    esp_wifi_stop();
    transitionTo(WiFiState::UNPROVISIONED_AP);
}

void WiFiManager::handleRuntimeSta()
{
    ctx->runtime->start();
}

WiFiManager* create(WiFiContext& ctx)
{
    auto* mgr = new WiFiManager(&ctx);
    ctx.manager = mgr;
    return mgr;
}

void WiFiManager::loop()
{
    // No logic here. Only heart beat, metrics, watchdog  
    static uint32_t hb = 0;
    if ((hb++ % 200) == 0) {
        ESP_LOGD(TAG, "[HB] WiFiManager alive, state=%d", (int)ctx->state);
    }
}

}