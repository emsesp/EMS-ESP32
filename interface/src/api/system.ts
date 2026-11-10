import type { ConsoleOutput, LogSettings, SystemStatus } from 'types';

import { alovaInstance } from './endpoints';

// systemStatus - also used in System Monitor to ping the ESP
export const readSystemStatus = () =>
  alovaInstance.Get<SystemStatus>('/rest/systemStatus');

export const readCoreDump = () =>
  alovaInstance.Get<Blob>('/rest/coredump', { responseType: 'blob' });

// SystemLog
export const readLogSettings = () =>
  alovaInstance.Get<LogSettings>('/rest/logSettings');
export const updateLogSettings = (data: LogSettings) =>
  alovaInstance.Post('/rest/logSettings', data);
export const fetchLogES = () => alovaInstance.Get('/es/log');
export const runConsoleCommand = (command: string) =>
  alovaInstance.Post<ConsoleOutput>('/rest/console', { command });

const UPLOAD_TIMEOUT = 60000; // 1 minute

// custom: the user agreed to install a build that isn't an official EMS-ESP build
export const uploadFile = (file: File, custom = false) => {
  const formData = new FormData();
  formData.append('file', file);
  return alovaInstance.Post('/rest/uploadFile', formData, {
    timeout: UPLOAD_TIMEOUT,
    ...(custom && { params: { custom: 1 } })
  });
};
