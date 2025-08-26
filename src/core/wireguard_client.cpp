#include "wireguard_client.h"

#ifndef EMSESP_STANDALONE
#include "../ESP32React/WireGuardSettingsService.h"
#endif

#ifndef EMSESP_STANDALONE

namespace emsesp {

void WireGuardClient::begin(const WireGuardSettings & settings) {
    if (!settings.enabled || settings.private_key.isEmpty() || settings.peer_public_key.isEmpty() || settings.endpoint.isEmpty()
        || settings.address.isEmpty()) {
        return;
    }

    config_.private_key        = settings.private_key.c_str();
    config_.public_key         = settings.peer_public_key.c_str();
    config_.preshared_key      = settings.preshared_key.c_str();
    config_.address            = settings.address.c_str();
    config_.netmask            = settings.netmask.c_str();
    config_.endpoint           = settings.endpoint.c_str();
    config_.port               = settings.port;
    config_.persistent_keepalive = settings.persistent_keepalive;

    if (esp_wireguard_init(&config_, &ctx_) != ESP_OK) {
        return;
    }
    esp_wireguard_connect(&ctx_);
    initialized_ = true;
}

void WireGuardClient::loop() {
    if (!initialized_) {
        return;
    }

    unsigned long now = millis();
    if (esp_wireguard_peer_is_up(&ctx_) != ESP_OK) {
        if (now - last_retry_ > retry_interval_) {
            esp_wireguard_connect(&ctx_);
            last_retry_ = now;
        }
        return;
    }

    time_t last_handshake = 0;
    if (esp_wireguard_latest_handshake(&ctx_, &last_handshake) == ESP_OK) {
        if (time(nullptr) - last_handshake > (WIREGUARD_KEEPALIVE * 3)) {
            if (now - last_retry_ > retry_interval_) {
                esp_wireguard_connect(&ctx_);
                last_retry_ = now;
            }
        }
    }
}

} // namespace emsesp

#endif

