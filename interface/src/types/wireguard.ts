export interface WireGuardSettingsType {
  enabled: boolean;
  private_key: string;
  peer_public_key: string;
  preshared_key: string;
  address: string;
  netmask: string;
  endpoint: string;
  port: number;
  persistent_keepalive: number;
}

