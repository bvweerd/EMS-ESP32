import Schema from 'async-validator';
import type { WireGuardSettingsType } from 'types';

export const createWireGuardSettingsValidator = (
  wg: WireGuardSettingsType
) =>
  new Schema({
    ...(wg.enabled && {
      private_key: { required: true, message: 'Private key is required' },
      peer_public_key: {
        required: true,
        message: 'Peer public key is required'
      },
      address: { required: true, message: 'Address is required' },
      endpoint: { required: true, message: 'Endpoint is required' },
      port: [
        { required: true, message: 'Port is required' },
        { type: 'number', min: 0, max: 65535, message: 'Invalid Port' }
      ],
      persistent_keepalive: [
        {
          type: 'number',
          min: 0,
          max: 65535,
          message: 'Invalid keepalive'
        }
      ]
    })
  });

