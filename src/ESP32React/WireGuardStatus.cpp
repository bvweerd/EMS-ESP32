#include "WireGuardStatus.h"

#include <emsesp.h>

WireGuardStatus::WireGuardStatus(AsyncWebServer * server, SecurityManager * securityManager, WireGuardSettingsService * service)
    : _wireguardSettingsService(service) {
    securityManager->addEndpoint(server, WIREGUARD_STATUS_SERVICE_PATH, AuthenticationPredicates::IS_AUTHENTICATED, [this](AsyncWebServerRequest * request) {
        wireguardStatus(request);
    });
}

void WireGuardStatus::wireguardStatus(AsyncWebServerRequest * request) {
    auto *     response = new AsyncJsonResponse(false);
    JsonObject root     = response->getRoot();

    _wireguardSettingsService->read([&](auto & settings) {
        root["enabled"]              = settings.enabled;
        root["endpoint"]             = settings.endpoint;
        root["address"]              = settings.address;
        root["persistent_keepalive"] = settings.persistent_keepalive;
    });

    root["connected"] = emsesp::EMSESP::system_.wireguard_connected();
    time_t handshake   = emsesp::EMSESP::system_.wireguard_latest_handshake();
    if (handshake) {
        root["latest_handshake"] = handshake;
    }

    response->setLength();
    request->send(response);
}
