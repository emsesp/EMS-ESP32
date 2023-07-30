import { alovaInstance, alovaInstanceGH } from './endpoints';
import type { OTASettings, SystemStatus, LogSettings, Version } from 'types';

// SystemStatus - also used to ping in Restart monitor for pinging
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
  alovaInstanceGH.Get<Version>('releases/latest', {
    transformData(response: any) {
      return {
        version: response.data.name,
        url: response.data.assets[1].browser_download_url,
        changelog: response.data.assets[0].browser_download_url
      };
    }
  });

export const getDevVersion = () =>
  alovaInstanceGH.Get<Version>('releases/tags/latest', {
    transformData(response: any) {
      return {
        version: response.data.name.split(/\s+/).splice(-1),
        url: response.data.assets[1].browser_download_url,
        changelog: response.data.assets[0].browser_download_url
      };
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
