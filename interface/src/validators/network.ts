import Schema from 'async-validator';
import type { NetworkSettingsType } from 'types';

import { HOSTNAME_VALIDATOR, IP_ADDRESS_VALIDATOR } from './shared';

export const createNetworkSettingsValidator = (
  networkSettings: NetworkSettingsType
) =>
  new Schema({
    ssid: [
      { type: 'string', max: 32, message: 'SSID must be 32 characters or less' }
    ],
    bssid: [
      {
        type: 'string',
        max: 17,
        message: 'BSSID must be 17 characters or empty'
      }
    ],
    password: {
      type: 'string',
      max: 64,
      message: 'Password must be 64 characters or less'
    },
    hostname: [
      { required: true, message: 'Hostname is required' },
      HOSTNAME_VALIDATOR
    ],
    ...(networkSettings.static_ip_config && {
      local_ip: [
        { required: true, message: 'Local IP is required' },
        IP_ADDRESS_VALIDATOR
      ],
      gateway_ip: [
        { required: true, message: 'Gateway IP is required' },
        IP_ADDRESS_VALIDATOR
      ],
      subnet_mask: [
        { required: true, message: 'Subnet mask is required' },
        IP_ADDRESS_VALIDATOR
      ],
      dns_ip_1: IP_ADDRESS_VALIDATOR,
      dns_ip_2: IP_ADDRESS_VALIDATOR
    }),
      ...(networkSettings.wireguard_enabled && {
        wireguard_endpoint: [
          { required: true, message: 'WireGuard endpoint is required' }
        ],
        wireguard_port: [
          {
            type: 'number',
            required: true,
            min: 1,
            max: 65535,
            message: 'WireGuard port must be between 1 and 65535'
          }
        ],
        wireguard_address: [
          { required: true, message: 'WireGuard address is required' },
          IP_ADDRESS_VALIDATOR
        ],
        wireguard_netmask: [
          { required: true, message: 'WireGuard netmask is required' },
          IP_ADDRESS_VALIDATOR
        ],
        wireguard_private_key: [
          { required: true, message: 'WireGuard private key is required' }
        ],
        wireguard_peer_public_key: [
          { required: true, message: 'WireGuard peer public key is required' }
        ]
      })
    });
