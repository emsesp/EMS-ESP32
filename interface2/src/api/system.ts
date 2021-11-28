import { AxiosPromise } from 'axios';

import { OTASettings, SystemStatus, LogSettings, LogEntries } from '../types';

import { AXIOS, AXIOS_BIN, FileUploadConfig, uploadFile } from './endpoints';

export function readSystemStatus(): AxiosPromise<SystemStatus> {
  return AXIOS.get('/systemStatus');
}

export function restart(): AxiosPromise<void> {
  return AXIOS.post('/restart');
}

export function factoryReset(): AxiosPromise<void> {
  return AXIOS.post('/factoryReset');
}

export function readOTASettings(): AxiosPromise<OTASettings> {
  return AXIOS.get('/otaSettings');
}

export function updateOTASettings(otaSettings: OTASettings): AxiosPromise<OTASettings> {
  return AXIOS.post('/otaSettings', otaSettings);
}

export const uploadFirmware = (file: File, config?: FileUploadConfig): AxiosPromise<void> =>
  uploadFile('/uploadFirmware', file, config);

export function readLogSettings(): AxiosPromise<LogSettings> {
  return AXIOS.get('/logSettings');
}

export function updateLogSettings(logSettings: LogSettings): AxiosPromise<LogSettings> {
  return AXIOS.post('/logSettings', logSettings);
}

export function readLogEntries(): AxiosPromise<LogEntries> {
  return AXIOS_BIN.get('/fetchLog');
}
