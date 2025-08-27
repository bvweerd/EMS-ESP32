#include "wireguard_client.h"

#ifndef EMSESP_STANDALONE
#include "../ESP32React/WireGuardSettingsService.h"
#include "common.h"
#include <esp_err.h>
#include <lwip/ip_addr.h>
#include <lwip/inet.h>
#include <lwip/ip4_addr.h>
#include <lwip/err.h>

namespace emsesp {

uuid::log::Logger WireGuardClient::logger_{F_(wireguard), uuid::log::Facility::DAEMON};

void WireGuardClient::begin(const WireGuardSettings & settings) {
    enabled_ = settings.enabled;
    if (!settings.enabled || settings.private_key.isEmpty() || settings.peer_public_key.isEmpty() || settings.endpoint.isEmpty() || settings.address.isEmpty()) {
        LOG_INFO("WireGuard disabled or incomplete configuration");
        enabled_ = false;
        return;
    }

    // reset structures to known defaults before filling
    config_ = ESP_WIREGUARD_CONFIG_DEFAULT();
    ctx_    = ESP_WIREGUARD_CONTEXT_DEFAULT();

    config_.private_key = settings.private_key.c_str();
    config_.public_key  = settings.peer_public_key.c_str();
    if (!settings.preshared_key.isEmpty()) {
        config_.preshared_key = settings.preshared_key.c_str();
    }
    config_.address              = settings.address.c_str();
    config_.netmask              = settings.netmask.c_str();
    config_.endpoint             = settings.endpoint.c_str();
    config_.port                 = settings.port;
    config_.persistent_keepalive = settings.persistent_keepalive;

    initialized_      = false;
    connected_        = false;
    latest_handshake_ = 0;
}

void WireGuardClient::start() {
    if (!enabled_ || initialized_) {
        return;
    }

    LOG_INFO("Starting WireGuard service");
    if (esp_wireguard_init(&config_, &ctx_) != ESP_OK) {
        LOG_ERROR("WireGuard init failed");
        return;
    }
    LOG_INFO("Connecting to %s:%u", config_.endpoint, config_.port);
    ip_addr_set_zero(&config_.endpoint_ip);
    esp_err_t err = esp_wireguard_connect(&ctx_);
    if (err == ESP_ERR_RETRY) {
        LOG_INFO("WireGuard awaiting DNS resolution for %s", config_.endpoint);
    } else if (err == ESP_ERR_INVALID_IP) {
        LOG_WARNING("WireGuard endpoint %s could not be resolved", config_.endpoint);
    } else if (err != ESP_OK) {
        if (err < 0) {
            LOG_ERROR("WireGuard connect failed: %s (%d) %s", esp_err_to_name(err), err, lwip_strerr((err_t)err));
        } else {
            LOG_ERROR("WireGuard connect failed: %s (%d)", esp_err_to_name(err), err);
        }
    }
    initialized_      = true;
    connected_        = false;
    latest_handshake_ = 0;
    last_retry_       = millis();
}

void WireGuardClient::stop() {
    if (!initialized_) {
        return;
    }

    esp_wireguard_disconnect(&ctx_);
    initialized_      = false;
    connected_        = false;
    latest_handshake_ = 0;
    LOG_INFO("WireGuard stopped");
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
        // Ensure routing to the WireGuard network exists once the tunnel is up.
        ip_addr_t addr, mask, network;
        if (ipaddr_aton(config_.address, &addr) == 1 && ipaddr_aton(config_.netmask, &mask) == 1) {
            network.type      = IPADDR_TYPE_V4;
            network.u_addr.ip4.addr = ip_2_ip4(&addr)->addr & ip_2_ip4(&mask)->addr;
            char network_str[IP4ADDR_STRLEN_MAX];
            ip4addr_ntoa_r(ip_2_ip4(&network), network_str, sizeof(network_str));
            esp_err_t route_err = esp_wireguard_add_allowed_ip(&ctx_, network_str, config_.netmask);
            if (route_err != ESP_OK) {
                LOG_WARNING("WireGuard allow route failed: %s (%d)", esp_err_to_name(route_err), route_err);
            }
        }
    } else if (!up && connected_) {
        connected_ = false;
        LOG_WARNING("WireGuard connection lost");
    }

    if (!up) {
        if (now - last_retry_ > retry_interval_) {
            LOG_INFO("Reconnecting WireGuard");
            ip_addr_set_zero(&config_.endpoint_ip);
            esp_err_t err = esp_wireguard_connect(&ctx_);
            if (err == ESP_ERR_RETRY) {
                LOG_INFO("WireGuard awaiting DNS resolution for %s", config_.endpoint);
            } else if (err == ESP_ERR_INVALID_IP) {
                LOG_WARNING("WireGuard endpoint %s could not be resolved", config_.endpoint);
            } else if (err != ESP_OK) {
                if (err < 0) {
                    LOG_WARNING("WireGuard reconnect failed: %s (%d) %s", esp_err_to_name(err), err, lwip_strerr((err_t)err));
                } else {
                    LOG_WARNING("WireGuard reconnect failed: %s (%d)", esp_err_to_name(err), err);
                }
            }
            last_retry_ = now;
        }
        return;
    }

    time_t last_handshake = 0;
    if (esp_wireguard_latest_handshake(&ctx_, &last_handshake) == ESP_OK) {
        latest_handshake_ = last_handshake;
    }
}

} // namespace emsesp

#endif
