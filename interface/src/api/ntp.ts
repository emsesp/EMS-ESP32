import { AXIOS } from './endpoints';
import type { AxiosPromise } from 'axios';
import type { NTPSettings, NTPStatus, Time } from 'types';

export function readNTPStatus(): AxiosPromise<NTPStatus> {
  return AXIOS.get('/ntpStatus');
}

export function readNTPSettings(): AxiosPromise<NTPSettings> {
  return AXIOS.get('/ntpSettings');
}

export function updateNTPSettings(ntpSettings: NTPSettings): AxiosPromise<NTPSettings> {
  return AXIOS.post('/ntpSettings', ntpSettings);
}

export function updateTime(time: Time): AxiosPromise<Time> {
  return AXIOS.post('/time', time);
}
