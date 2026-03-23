#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_spiffs.h"

#include "ApplicationContext.hpp"

static const char* TAG = "app_main";

extern "C" void app_main(void)
{
    // Logging
    esp_log_level_set("*", ESP_LOG_INFO);

    esp_log_level_set("ApplicationContext", ESP_LOG_DEBUG);
    esp_log_level_set("RuntimeServer", ESP_LOG_DEBUG);
    esp_log_level_set("ProvisioningServer", ESP_LOG_DEBUG);
    esp_log_level_set("ProvisioningApiHandler", ESP_LOG_DEBUG);
    esp_log_level_set("serveEmbedded", ESP_LOG_DEBUG);
    esp_log_level_set("EmbeddedFiles", ESP_LOG_DEBUG);
    esp_log_level_set("ApplicationContext", ESP_LOG_DEBUG);
    esp_log_level_set("CredentialApiHandler", ESP_LOG_DEBUG);
    esp_log_level_set("WiFiApiHandler", ESP_LOG_DEBUG);

    ESP_LOGI(TAG, "Booting…");

    // Core ESP-IDF init
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    static ApplicationContext ctx;
    ctx.start();

    ESP_LOGI("app_main", "System initialised");
    
}