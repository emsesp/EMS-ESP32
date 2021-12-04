import { AxiosPromise } from 'axios';
import { AXIOS, AXIOS_API, AXIOS_BIN } from '../api/endpoints';

import { BoardProfile, APIcall, Settings, Status, Data, DeviceData, DeviceID, WriteValue, WriteSensor } from './types';

export function restart(): AxiosPromise<void> {
  return AXIOS.post('/restart');
}

export function readSettings(): AxiosPromise<Settings> {
  return AXIOS.get('/settings');
}

export function updateSettings(settings: Settings): AxiosPromise<Settings> {
  return AXIOS.post('/settings', settings);
}

export function updateBoardProfile(boardProfile: BoardProfile): AxiosPromise<void> {
  return AXIOS.post('/boardProfile', boardProfile);
}

export function readStatus(): AxiosPromise<Status> {
  return AXIOS.get('/status');
}

export function readData(): AxiosPromise<Data> {
  return AXIOS.get('/data');
}

export function scanDevices(): AxiosPromise<void> {
  return AXIOS.post('/scanDevices');
}

export function readDeviceData(deviceid: DeviceID): AxiosPromise<DeviceData> {
  return AXIOS_BIN.post('/deviceData', deviceid);
}

export function writeValue(writevalue: WriteValue): AxiosPromise<void> {
  return AXIOS.post('/writeValue', writevalue);
}

export function writeSensor(writesensor: WriteSensor): AxiosPromise<void> {
  return AXIOS.post('/writeSensor', writesensor);
}

// EMS-ESP API calls
export function API(apiCall: APIcall): AxiosPromise<void> {
  return AXIOS_API.post('/', apiCall);
}
