import type { WireGuardSettingsType } from 'types';

import { alovaInstance } from './endpoints';

export const readWireGuardSettings = () =>
  alovaInstance.Get<WireGuardSettingsType>('/rest/wireguardSettings');
export const updateWireGuardSettings = (data: WireGuardSettingsType) =>
  alovaInstance.Post<WireGuardSettingsType>('/rest/wireguardSettings', data);

