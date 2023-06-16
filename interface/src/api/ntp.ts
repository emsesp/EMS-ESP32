import { alovaInstance } from './endpoints';
import type { NTPSettings, NTPStatus, Time } from 'types';

export const readNTPStatus = () => alovaInstance.Get<NTPStatus>('/rest/ntpStatus');
export const readNTPSettings = () =>
  alovaInstance.Get<NTPSettings>('/rest/ntpSettings', {
    name: 'ntpSettings'
  });
export const updateNTPSettings = (data: NTPSettings) => alovaInstance.Post<NTPSettings>('/rest/ntpSettings', data);

export const updateTime = (data: Time) => alovaInstance.Post<Time>('/rest/time', data);
