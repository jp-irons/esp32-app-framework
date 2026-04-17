#pragma once

#include <cstdint>
#include <functional>
#include "esp_timer.h"

namespace device {

class DeferredExecutor {
public:
    DeferredExecutor();
    ~DeferredExecutor();

    // Schedule a callback to run once after delayMs milliseconds.
    // Safe to call from ISR, WiFi event handlers, or tasks.
    void runAfter(uint32_t delayMs, std::function<void()> fn);

private:
    static void timerCallback(void *arg);

    esp_timer_handle_t timerHandle{};
    std::function<void()> callback;
};

} // namespace device
