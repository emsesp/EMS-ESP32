/* eslint-disable @typescript-eslint/no-unsafe-member-access */

/* eslint-disable @typescript-eslint/no-unsafe-call */

/* eslint-disable @typescript-eslint/no-unsafe-return */
import type { HardwareStatus, LogSettings, SystemStatus } from 'types';

import { alovaInstance, alovaInstanceGH } from './endpoints';

// HardwareStatus - also used to ping in Restart monitor for pinging
export const readHardwareStatus = () =>
  alovaInstance.Get<HardwareStatus>('/rest/HardwareStatus');

// SystemStatus
export const readSystemStatus = () =>
  alovaInstance.Get<SystemStatus>('/rest/systemStatus');

// commands
export const restart = () => alovaInstance.Post('/rest/restart');
export const partition = () => alovaInstance.Post('/rest/partition');
export const factoryReset = () => alovaInstance.Post('/rest/factoryReset');

// SystemLog
export const readLogSettings = () =>
  alovaInstance.Get<LogSettings>(`/rest/logSettings`);
export const updateLogSettings = (data: LogSettings) =>
  alovaInstance.Post('/rest/logSettings', data);
export const fetchLog = () => alovaInstance.Post('/rest/fetchLog');
export const fetchLogES = () => alovaInstance.Get('/es/log');

// Get versions from github
export const getStableVersion = () =>
  alovaInstanceGH.Get('latest', {
    transformData(response) {
      return response.data.name.substring(1);
    }
  });
export const getDevVersion = () =>
  alovaInstanceGH.Get('tags/latest', {
    transformData(response) {
      return response.data.name.split(/\s+/).splice(-1)[0].substring(1);
    }
  });

export const uploadFile = (file: File) => {
  const formData = new FormData();
  formData.append('file', file);
  return alovaInstance.Post('/rest/uploadFile', formData, {
    timeout: 60000 // override timeout for uploading firmware - 1 minute
  });
};
