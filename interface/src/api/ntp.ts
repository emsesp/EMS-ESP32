import type { NTPSettingsType, NTPStatusType, Time } from 'types';

import { alovaInstance } from './endpoints';

export const readNTPStatus = () =>
  alovaInstance.Get<NTPStatusType>('/rest/ntpStatus');

export const readNTPSettings = () =>
  alovaInstance.Get<NTPSettingsType>('/rest/ntpSettings', {});
export const updateNTPSettings = (data: NTPSettingsType) =>
  alovaInstance.Post<NTPSettingsType>('/rest/ntpSettings', data);

export const updateTime = (data: Time) =>
  alovaInstance.Post<Time>('/rest/time', data);
