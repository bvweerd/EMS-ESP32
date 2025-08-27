#include "WireGuardSettingsService.h"

#include <emsesp.h>

WireGuardSettingsService::WireGuardSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _httpEndpoint(WireGuardSettings::read, WireGuardSettings::update, this, server, WIREGUARD_SETTINGS_SERVICE_PATH, securityManager)
    , _fsPersistence(WireGuardSettings::read, WireGuardSettings::update, this, fs, WIREGUARD_SETTINGS_FILE) {
}

void WireGuardSettingsService::begin() {
    _fsPersistence.readFromFS();
}

void WireGuardSettings::read(WireGuardSettings & settings, JsonObject root) {
    root["enabled"]              = settings.enabled;
    root["private_key"]          = settings.private_key;
    root["peer_public_key"]      = settings.peer_public_key;
    root["preshared_key"]        = settings.preshared_key;
    root["address"]              = settings.address;
    root["netmask"]              = settings.netmask;
    root["endpoint"]             = settings.endpoint;
    root["port"]                 = settings.port;
    root["persistent_keepalive"] = settings.persistent_keepalive;
}

StateUpdateResult WireGuardSettings::update(JsonObject root, WireGuardSettings & settings) {
    auto enabled         = settings.enabled;
    auto endpoint        = settings.endpoint;
    auto address         = settings.address;

    settings.enabled              = root["enabled"] | FACTORY_WIREGUARD_ENABLED;
    settings.private_key          = root["private_key"] | FACTORY_WIREGUARD_PRIVATE_KEY;
    settings.peer_public_key      = root["peer_public_key"] | FACTORY_WIREGUARD_PEER_PUBLIC_KEY;
    settings.preshared_key        = root["preshared_key"] | FACTORY_WIREGUARD_PRESHARED_KEY;
    settings.address              = root["address"] | FACTORY_WIREGUARD_ADDRESS;
    settings.netmask              = root["netmask"] | FACTORY_WIREGUARD_NETMASK;
    settings.endpoint             = root["endpoint"] | FACTORY_WIREGUARD_ENDPOINT;
    settings.port                 = root["port"] | FACTORY_WIREGUARD_PORT;
    settings.persistent_keepalive = root["persistent_keepalive"] | FACTORY_WIREGUARD_KEEPALIVE;

    if (enabled != settings.enabled || endpoint != settings.endpoint || address != settings.address) {
        return StateUpdateResult::CHANGED_RESTART;
    }

    return StateUpdateResult::CHANGED;
}

