#ifndef WireGuardStatus_h
#define WireGuardStatus_h

#define WIREGUARD_STATUS_SERVICE_PATH "/rest/wireguardStatus"

#include "WireGuardSettingsService.h"
#include "SecurityManager.h"
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

class WireGuardStatus {
  public:
    WireGuardStatus(AsyncWebServer * server, SecurityManager * securityManager, WireGuardSettingsService * service);

  private:
    void wireguardStatus(AsyncWebServerRequest * request);
    WireGuardSettingsService * _wireguardSettingsService;
};

#endif
