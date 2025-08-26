/*
 * WireGuard client wrapper for EMS-ESP32
 */

#pragma once

#include <Arduino.h>
#ifndef EMSESP_STANDALONE
#include <esp_wireguard.h>
#endif
#include <uuid/log.h>

class WireGuardSettings;

#ifndef EMSESP_STANDALONE

#ifndef WIREGUARD_PRIVATE_KEY
#define WIREGUARD_PRIVATE_KEY ""
#endif

#ifndef WIREGUARD_PEER_PUBLIC_KEY
#define WIREGUARD_PEER_PUBLIC_KEY ""
#endif

#ifndef WIREGUARD_PRESHARED_KEY
#define WIREGUARD_PRESHARED_KEY ""
#endif

#ifndef WIREGUARD_ADDRESS
#define WIREGUARD_ADDRESS ""
#endif

#ifndef WIREGUARD_NETMASK
#define WIREGUARD_NETMASK "255.255.255.255"
#endif

#ifndef WIREGUARD_ENDPOINT
#define WIREGUARD_ENDPOINT ""
#endif

#ifndef WIREGUARD_PORT
#define WIREGUARD_PORT 51820
#endif

#ifndef WIREGUARD_KEEPALIVE
#define WIREGUARD_KEEPALIVE 25
#endif

namespace emsesp {

class WireGuardClient {
  public:
    void begin(const WireGuardSettings & settings);
    void loop();
    bool   enabled() const { return enabled_; }
    bool   connected() const { return connected_; }
    time_t latest_handshake() const { return latest_handshake_; }

  private:
    wireguard_config_t config_ = ESP_WIREGUARD_CONFIG_DEFAULT();
    wireguard_ctx_t    ctx_    = ESP_WIREGUARD_CONTEXT_DEFAULT();
    bool               initialized_ = false;
    bool               enabled_     = false;
    bool               connected_   = false;
    time_t             latest_handshake_ = 0;
    unsigned long      last_retry_  = 0;
    static constexpr unsigned long retry_interval_ = 5000; // ms

    static uuid::log::Logger logger_;
};

} // namespace emsesp

#else // EMSESP_STANDALONE

namespace emsesp {

class WireGuardClient {
  public:
    template <typename T>
    void begin(const T &) {}
    void loop() {}
    bool   enabled() const { return false; }
    bool   connected() const { return false; }
    time_t latest_handshake() const { return 0; }
};

} // namespace emsesp

#endif

