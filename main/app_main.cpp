#include "ApplicationContext.hpp"
#include "esp_log.h"

extern "C" {
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
}

extern "C" void setupLogging();

static const char *TAG = "app_main";

extern "C" void app_main(void) {
    // Logging
    setupLogging();
    // Create the application context (owns everything)
    ESP_LOGD(TAG, "creating app context");
    static ApplicationContext app;
    app.start();
    ESP_LOGI(TAG, "System initialised");

    // Main loop
    while (true) {
        app.loop();
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

extern "C" void setupLogging() {
    esp_log_level_set("app_main", ESP_LOG_DEBUG);
    esp_log_level_set("ApplicationContext", ESP_LOG_DEBUG);
    // core_api API Handlers
    esp_log_level_set("CredentialApiHandler", ESP_LOG_DEBUG);
    esp_log_level_set("WiFiApiHandler", ESP_LOG_DEBUG);
    // credential_store
    esp_log_level_set("CredentialStore", ESP_LOG_DEBUG);
    // framework
    esp_log_level_set("FrameworkContext", ESP_LOG_DEBUG);
    // http
    esp_log_level_set("HttpServer", ESP_LOG_DEBUG);
    // wifi_manager
    esp_log_level_set("ProvisioningServer", ESP_LOG_DEBUG);
    esp_log_level_set("RuntimeServer", ESP_LOG_DEBUG);
    esp_log_level_set("WiFiInterface", ESP_LOG_DEBUG);
    esp_log_level_set("WiFiStateMachine", ESP_LOG_DEBUG);
    // static_assets
    esp_log_level_set("ContentType", ESP_LOG_DEBUG);
    esp_log_level_set("EmbeddedAssetTable", ESP_LOG_DEBUG);
    esp_log_level_set("StaticFileHandler", ESP_LOG_DEBUG);
}

