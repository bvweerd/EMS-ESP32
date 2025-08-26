import type { WireGuardSettingsType } from 'types';
import type { WireGuardStatusType } from 'types';

import { alovaInstance } from './endpoints';

export const readWireGuardSettings = () =>
  alovaInstance.Get<WireGuardSettingsType>('/rest/wireguardSettings');
export const updateWireGuardSettings = (data: WireGuardSettingsType) =>
  alovaInstance.Post<WireGuardSettingsType>('/rest/wireguardSettings', data);
export const readWireGuardStatus = () =>
  alovaInstance.Get<WireGuardStatusType>('/rest/wireguardStatus');

