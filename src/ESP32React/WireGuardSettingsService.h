#ifndef WireGuardSettingsService_h
#define WireGuardSettingsService_h

#include "StatefulService.h"
#include "HttpEndpoint.h"
#include "FSPersistence.h"

#define WIREGUARD_SETTINGS_FILE "/config/wireguardSettings.json"
#define WIREGUARD_SETTINGS_SERVICE_PATH "/rest/wireguardSettings"

#ifndef FACTORY_WIREGUARD_ENABLED
#define FACTORY_WIREGUARD_ENABLED false
#endif

#ifndef FACTORY_WIREGUARD_PRIVATE_KEY
#define FACTORY_WIREGUARD_PRIVATE_KEY ""
#endif

#ifndef FACTORY_WIREGUARD_PEER_PUBLIC_KEY
#define FACTORY_WIREGUARD_PEER_PUBLIC_KEY ""
#endif

#ifndef FACTORY_WIREGUARD_PRESHARED_KEY
#define FACTORY_WIREGUARD_PRESHARED_KEY ""
#endif

#ifndef FACTORY_WIREGUARD_ADDRESS
#define FACTORY_WIREGUARD_ADDRESS ""
#endif

#ifndef FACTORY_WIREGUARD_NETMASK
#define FACTORY_WIREGUARD_NETMASK "255.255.255.255"
#endif

#ifndef FACTORY_WIREGUARD_ENDPOINT
#define FACTORY_WIREGUARD_ENDPOINT ""
#endif

#ifndef FACTORY_WIREGUARD_PORT
#define FACTORY_WIREGUARD_PORT 51820
#endif

#ifndef FACTORY_WIREGUARD_KEEPALIVE
#define FACTORY_WIREGUARD_KEEPALIVE 25
#endif

class WireGuardSettings {
  public:
    bool     enabled;
    String   private_key;
    String   peer_public_key;
    String   preshared_key;
    String   address;
    String   netmask;
    String   endpoint;
    uint16_t port;
    uint16_t persistent_keepalive;

    static void              read(WireGuardSettings & settings, JsonObject root);
    static StateUpdateResult update(JsonObject root, WireGuardSettings & settings);
};

class WireGuardSettingsService : public StatefulService<WireGuardSettings> {
  public:
    WireGuardSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager);

    void begin();

  private:
    HttpEndpoint<WireGuardSettings>  _httpEndpoint;
    FSPersistence<WireGuardSettings> _fsPersistence;
};

#endif

