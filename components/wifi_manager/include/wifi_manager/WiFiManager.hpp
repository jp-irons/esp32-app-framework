// SPDX-FileCopyrightText: 2026 Jon Irons <jon@irons.ws>
// SPDX-License-Identifier: MIT

#pragma once

#include "wifi_manager/WiFiContext.hpp"
#include "wifi_manager/WiFiStateMachine.hpp"
#include "wifi_manager/WiFiTypes.hpp"

namespace wifi_manager {

class WiFiManager {
  public:
    static constexpr const char* TAG = "WiFiManager";

    explicit WiFiManager(WiFiContext &ctx);

    void start();
    void loop();

    // State machine integration
    void onStateChanged(WiFiState oldState, WiFiState newState);

    // ESP-IDF event callbacks
    void onConnectSuccess();
    void onConnectFail();

    /**
     * @param reason Classified cause of the disconnect, when known (from a
     *               real WIFI_EVENT_STA_DISCONNECTED). Defaults to
     *               WiFiError::UNKNOWN for callers that don't have a
     *               specific reason (IP_EVENT_STA_LOST_IP, onConnectTimeout(),
     *               forceReconnect()). The reason only affects backoff
     *               timing between same-network retries (see
     *               retryDelayMsFor()) — the very first reconnect attempt
     *               against whatever's currently being tried gets an
     *               immediate soft WiFi driver reset regardless of reason;
     *               see driverResetCycleCount's doc comment for why.
     */
    void onDisconnect(WiFiError reason = WiFiError::UNKNOWN);
    void onFatalError();
    void onStaGotIp(const StaIpInfo &info);
    void onApStarted();
    WiFiStaStatus getStaStatus() const;

    /**
     * Forces a full STA disconnect/reconnect cycle (radio stop/start,
     * fresh esp_wifi_connect(), mDNS restart) via the same path a real
     * WIFI_EVENT_STA_DISCONNECTED would take.
     *
     * For app-level self-healing: WiFi/IP can look perfectly healthy
     * (associated, valid IP) while name resolution or connectivity to a
     * specific peer is silently wedged (e.g. a stuck mDNS cache) — nothing
     * in the WiFi event chain itself observes that, so nothing today would
     * ever trigger a recovery. Call this when app code has independently
     * detected a prolonged connectivity failure despite WiFi/IP appearing
     * fine.
     */
    void forceReconnect();

  private:
    WiFiContext &ctx;
    WiFiStateMachine sm;

    std::size_t currentNetworkIndex = 0;
	std::optional<network_store::WiFiNetwork> currentNetwork;
	int retryCount = 0;
    static constexpr int MAX_RETRIES = 3;

    // Soft driver-reset escalation: do a full WiFiInterface
    // stopDriver()/startDriver() (fresh esp_wifi_init(), fresh netifs, but
    // netifs/event-handler registrations are preserved across the cycle —
    // see EspWiFiInterface's own comments, fixed 2026-07-22 after a
    // coredump-confirmed use-after-free) before falling back to AP_Mode.
    // Field evidence 2026-07-21 (node170): a full esp_restart() reliably
    // connects on its very first attempt after the plain retry loop fails
    // continuously for 20+ minutes, strongly suggesting a fresh driver init
    // clears some stuck internal state that plain disconnect()/stop()/
    // start() (already done on every retry via connectSta()) never
    // touches. This tries for the same effect without the cost of a full
    // chip reboot — GPS lock, ESP-NOW state, and the audio ring buffer all
    // survive a driver-only reset.
    //
    // UPDATE 2026-07-22: originally this only fired once every network in
    // the list had exhausted its own MAX_RETRIES cycle (i.e. only as a
    // last resort). Now shared across two firing points against the same
    // capped budget: onDisconnect() spends the first cycle immediately, on
    // the very first sign of trouble for whatever's currently being
    // attempted — field evidence (node174, node171, same date) is that a
    // warm retry against an already-wedged driver has never once succeeded
    // for a genuine reason=15 handshake-timeout failure, so waiting for 3
    // guaranteed-failing retries first was pure latency, not caution that
    // was ever paying off. continueRetrySequence() still spends the second
    // (and final) cycle the old way, once a full 3-retry backoff ladder
    // against the freshly-reset driver has also failed. See project memory,
    // project_bird_wifi_reliability_investigation, for the full history.
    // Capped rather than unconditional so a genuinely dead/absent AP still
    // eventually reaches AP_Mode and the existing HubHeartbeat
    // esp_restart() safety net, rather than looping on driver resets
    // forever.
    int driverResetCycleCount = 0;
    static constexpr int MAX_DRIVER_RESET_CYCLES = 2;

    int driverRetryCount = 0;
    static constexpr int MAX_DRIVER_RETRIES = 3;
    static constexpr uint32_t DRIVER_RETRY_DELAY_MS = 3000;
    std::string lastErrorReason;

    // Watchdog for a single STA connection attempt — covers the case where
    // esp_wifi_connect() never resolves to either WIFI_EVENT_STA_CONNECTED or
    // WIFI_EVENT_STA_DISCONNECTED, leaving the state machine wedged in
    // STA_Connecting indefinitely.
    static constexpr uint32_t STA_CONNECT_TIMEOUT_MS = 20000;
    uint32_t connectAttemptId = 0;
    void onConnectTimeout(uint32_t attemptId);

    // Retry/next-network/exhausted branching, factored out of onDisconnect()
    // so it's reachable both from a normal disconnect and as the
    // continuation after onDisconnect()'s own immediate pre-retry driver
    // reset (see driverResetCycleCount's doc comment above).
    void continueRetrySequence(WiFiError reason);

    // Load networks only at boot
    void loadInitialNetwork();

    // Wi-Fi actions
    void startAP();
    void stopAP();
    void startSTA();
    void stopSTA();

    // Retry logic

    /**
     * Delay before the next same-network STA retry, given the reason for
     * the disconnect that triggered it and how many retries have already
     * been made this cycle (1-based — the retry about to be scheduled).
     * Returns 0 (immediate retry, today's behavior) for every reason except
     * WiFiError::HANDSHAKE_TIMEOUT.
     *
     * 2026-07-19 field investigation (repeated fleet-wide overnight
     * outages, see project memory) found nodes retrying STA every ~3s with
     * no backoff, every attempt failing with WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT,
     * for many hours — and the ESP32/ESP-IDF community's own experience
     * with this exact reason (github.com/espressif/arduino-esp32#7968) is
     * that immediate, repeated retry against the same AP after a handshake
     * timeout is often what sustains the failure: some APs apply their own
     * deauth/rate-limit protection against a client that keeps retrying
     * without pausing. Growing delay across the 3 same-network retries
     * gives that protection window a chance to lapse instead of retriggering
     * it every ~3 seconds. Other disconnect reasons are untouched — this
     * isn't a blanket slowdown, it targets the one failure mode with actual
     * evidence behind it.
     */
    static uint32_t retryDelayMsFor(WiFiError reason, int retryAttempt);

    void retryDriver();
};

} // namespace wifi_manager
