import type { APSettingsType, APStatusType } from 'types';

import { alovaInstance } from './endpoints';

export const readAPStatus = () => alovaInstance.Get<APStatusType>('/rest/apStatus');
export const readAPSettings = () =>
  alovaInstance.Get<APSettingsType>('/rest/apSettings');
export const updateAPSettings = (data: APSettingsType) =>
  alovaInstance.Post<APSettingsType>('/rest/apSettings', data);
