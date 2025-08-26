#include "wireguard_client.h"

#ifndef EMSESP_STANDALONE
#include "../ESP32React/WireGuardSettingsService.h"
#include "common.h"

namespace emsesp {

uuid::log::Logger WireGuardClient::logger_{F_(wireguard), uuid::log::Facility::DAEMON};

void WireGuardClient::begin(const WireGuardSettings & settings) {
    enabled_ = settings.enabled;
    if (!settings.enabled || settings.private_key.isEmpty() || settings.peer_public_key.isEmpty() || settings.endpoint.isEmpty()
        || settings.address.isEmpty()) {
        LOG_INFO("WireGuard disabled or incomplete configuration");
        return;
    }

    config_.private_key          = settings.private_key.c_str();
    config_.public_key           = settings.peer_public_key.c_str();
    config_.preshared_key        = settings.preshared_key.c_str();
    config_.address              = settings.address.c_str();
    config_.netmask              = settings.netmask.c_str();
    config_.endpoint             = settings.endpoint.c_str();
    config_.port                 = settings.port;
    config_.persistent_keepalive = settings.persistent_keepalive;

    LOG_INFO("Starting WireGuard service");
    if (esp_wireguard_init(&config_, &ctx_) != ESP_OK) {
        LOG_ERROR("WireGuard init failed");
        return;
    }
    LOG_INFO("Connecting to %s:%u", config_.endpoint, config_.port);
    esp_wireguard_connect(&ctx_);
    initialized_      = true;
    connected_        = false;
    latest_handshake_ = 0;
    last_retry_       = millis();
}

void WireGuardClient::loop() {
    if (!initialized_) {
        return;
    }

    unsigned long now = millis();
    bool          up  = (esp_wireguard_peer_is_up(&ctx_) == ESP_OK);
    if (up && !connected_) {
        connected_ = true;
        LOG_INFO("WireGuard connection established");
    } else if (!up && connected_) {
        connected_ = false;
        LOG_WARNING("WireGuard connection lost");
    }

    if (!up) {
        if (now - last_retry_ > retry_interval_) {
            LOG_INFO("Reconnecting WireGuard");
            esp_wireguard_connect(&ctx_);
            last_retry_ = now;
        }
        return;
    }

    time_t last_handshake = 0;
    if (esp_wireguard_latest_handshake(&ctx_, &last_handshake) == ESP_OK) {
        latest_handshake_ = last_handshake;
        if (time(nullptr) - last_handshake > (config_.persistent_keepalive * 3)) {
            if (now - last_retry_ > retry_interval_) {
                LOG_WARNING("WireGuard keepalive timeout, reconnecting");
                esp_wireguard_connect(&ctx_);
                last_retry_ = now;
            }
        }
    }
}

} // namespace emsesp

#endif
