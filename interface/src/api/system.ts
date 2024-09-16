import type { LogSettings, SystemStatus } from 'types';

import { alovaInstance, alovaInstanceGH } from './endpoints';

// systemStatus - also used to ping in Restart monitor for pinging
export const readSystemStatus = () =>
  alovaInstance.Get<SystemStatus>('/rest/systemStatus');

// SystemLog
export const readLogSettings = () =>
  alovaInstance.Get<LogSettings>(`/rest/logSettings`);
export const updateLogSettings = (data: LogSettings) =>
  alovaInstance.Post('/rest/logSettings', data);
export const fetchLogES = () => alovaInstance.Get('/es/log');

// Get versions from github
export const getStableVersion = () =>
  alovaInstanceGH.Get('latest', {
    transform(response: { data: { name: string } }) {
      return response.data.name.substring(1);
    }
  });
export const getDevVersion = () =>
  alovaInstanceGH.Get('tags/latest', {
    transform(response: { data: { name: string } }) {
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

export const uploadURL = (data: { url: string }) =>
  alovaInstance.Post('/rest/uploadURL', data);

export const checkUpgrade = (data: { version: string }) =>
  alovaInstance.Post('/rest/checkUpgrade', data);
