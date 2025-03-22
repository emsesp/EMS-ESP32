import type { LogSettings, SystemStatus } from 'types';

import { alovaInstance, alovaInstanceGH } from './endpoints';

// systemStatus - also used to ping in System Monitor for pinging
export const readSystemStatus = () =>
  alovaInstance.Get<SystemStatus>('/rest/systemStatus');

// SystemLog
export const readLogSettings = () =>
  alovaInstance.Get<LogSettings>(`/rest/logSettings`);
export const updateLogSettings = (data: LogSettings) =>
  alovaInstance.Post('/rest/logSettings', data);
export const fetchLogES = () => alovaInstance.Get('/es/log');

// Get versions from GitHub
// cache for 10 minutes to stop getting the IP blocked by GitHub
export const getStableVersion = () =>
  alovaInstanceGH.Get('latest', {
    cacheFor: 60 * 10 * 1000,
    transform(response: { data: { name: string; published_at: string } }) {
      return {
        name: response.data.name.substring(1),
        published_at: response.data.published_at
      };
    }
  });
export const getDevVersion = () =>
  alovaInstanceGH.Get('tags/latest', {
    cacheFor: 60 * 10 * 1000,
    transform(response: { data: { name: string; published_at: string } }) {
      return {
        name: response.data.name.split(/\s+/).splice(-1)[0].substring(1),
        published_at: response.data.published_at
      };
    }
  });

export const uploadFile = (file: File) => {
  const formData = new FormData();
  formData.append('file', file);
  return alovaInstance.Post('/rest/uploadFile', formData, {
    timeout: 60000 // override timeout for uploading firmware - 1 minute
  });
};
