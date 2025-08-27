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

export interface WireGuardStatusType {
  enabled: boolean;
  connected: boolean;
  endpoint: string;
  address: string;
  persistent_keepalive: number;
  latest_handshake?: number;
}

