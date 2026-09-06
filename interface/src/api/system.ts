import type { LogSettings, SystemStatus } from 'types';

import { alovaInstance } from './endpoints';

// systemStatus - also used in System Monitor to ping the ESP
export const readSystemStatus = () =>
  alovaInstance.Get<SystemStatus>('/rest/systemStatus');

// SystemLog
export const readLogSettings = () =>
  alovaInstance.Get<LogSettings>('/rest/logSettings');
export const updateLogSettings = (data: LogSettings) =>
  alovaInstance.Post('/rest/logSettings', data);
export const fetchLogES = () => alovaInstance.Get('/es/log');

const UPLOAD_TIMEOUT = 60000; // 1 minute

export const uploadFile = (file: File) => {
  const formData = new FormData();
  formData.append('file', file);
  return alovaInstance.Post('/rest/uploadFile', formData, {
    timeout: UPLOAD_TIMEOUT,
    enableUpload: true
  });
};
