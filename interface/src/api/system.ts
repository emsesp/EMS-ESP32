import { AXIOS, AXIOS_BIN, alovaInstance, startUploadFile } from './endpoints';
import type { FileUploadConfig } from './endpoints';
import type { AxiosPromise } from 'axios';

import type { OTASettings, SystemStatus, LogSettings, LogEntries } from 'types';

// TODO move to Alova
// TODO fix this next!

export const readSystemStatus = (timeout?: number) =>
  alovaInstance.Get<SystemStatus>('/rest/systemStatus', {
    params: { timeout }
  });

// export function readSystemStatus(timeout?: number): AxiosPromise<SystemStatus> {
//   return AXIOS.get('/systemStatus', { timeout });
// }

export function restart(): AxiosPromise<void> {
  return AXIOS.post('/restart');
}

export function partition(): AxiosPromise<void> {
  return AXIOS.post('/partition');
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

export const uploadFile = (file: File, config?: FileUploadConfig): AxiosPromise<void> =>
  startUploadFile('/uploadFile', file, config);

export function readLogSettings(): AxiosPromise<LogSettings> {
  return AXIOS.get('/logSettings');
}

export function updateLogSettings(logSettings: LogSettings): AxiosPromise<LogSettings> {
  return AXIOS.post('/logSettings', logSettings);
}

export function readLogEntries(): AxiosPromise<LogEntries> {
  return AXIOS_BIN.get('/fetchLog');
}
