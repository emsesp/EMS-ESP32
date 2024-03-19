import { alovaInstance, alovaInstanceGH } from './endpoints';
import type { OTASettings, SystemStatus, LogSettings, ESPSystemStatus } from 'types';

// ESPSystemStatus - also used to ping in Restart monitor for pinging
export const readESPSystemStatus = () => alovaInstance.Get<ESPSystemStatus>('/rest/ESPSystemStatus');

// SystemStatus
export const readSystemStatus = () => alovaInstance.Get<SystemStatus>('/rest/systemStatus');

// commands
export const restart = () => alovaInstance.Post('/rest/restart');
export const partition = () => alovaInstance.Post('/rest/partition');
export const factoryReset = () => alovaInstance.Post('/rest/factoryReset');

// OTA
export const readOTASettings = () => alovaInstance.Get<OTASettings>(`/rest/otaSettings`);
export const updateOTASettings = (data: any) => alovaInstance.Post('/rest/otaSettings', data);

// SystemLog
export const readLogSettings = () => alovaInstance.Get<LogSettings>(`/rest/logSettings`);
export const updateLogSettings = (data: any) => alovaInstance.Post('/rest/logSettings', data);
export const fetchLog = () => alovaInstance.Post('/rest/fetchLog');

// Get versions from github
export const getStableVersion = () =>
  alovaInstanceGH.Get('latest', {
    transformData(response: any) {
      return response.data.name.substring(1);
    }
  });
export const getDevVersion = () =>
  alovaInstanceGH.Get('tags/latest', {
    transformData(response: any) {
      return response.data.name.split(/\s+/).splice(-1)[0].substring(1);
    }
  });

export const uploadFile = (file: File) => {
  const formData = new FormData();
  formData.append('file', file);
  return alovaInstance.Post('/rest/uploadFile', formData, {
    timeout: 60000, // override timeout for uploading firmware - 1 minute
    enableUpload: true
  });
};
