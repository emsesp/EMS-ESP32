import { AxiosPromise } from 'axios';
import { AXIOS, AXIOS_API, AXIOS_BIN } from '../api/endpoints';

import {
  BoardProfile,
  BoardProfileName,
  APIcall,
  Settings,
  Status,
  CoreData,
  Devices,
  DeviceData,
  DeviceEntity,
  UniqueID,
  ExcludeEntities,
  WriteValue,
  WriteSensor,
  WriteAnalog,
  SensorData
} from './types';

export function restart(): AxiosPromise<void> {
  return AXIOS.post('/restart');
}

export function readSettings(): AxiosPromise<Settings> {
  return AXIOS.get('/settings');
}

export function writeSettings(settings: Settings): AxiosPromise<Settings> {
  return AXIOS.post('/settings', settings);
}

export function getBoardProfile(boardProfile: BoardProfileName): AxiosPromise<BoardProfile> {
  return AXIOS.post('/boardProfile', boardProfile);
}

export function readStatus(): AxiosPromise<Status> {
  return AXIOS.get('/status');
}

export function readCoreData(): AxiosPromise<CoreData> {
  return AXIOS.get('/coreData');
}

export function readDevices(): AxiosPromise<Devices> {
  return AXIOS.get('/devices');
}

export function scanDevices(): AxiosPromise<void> {
  return AXIOS.post('/scanDevices');
}

export function readDeviceData(unique_id: UniqueID): AxiosPromise<DeviceData> {
  return AXIOS_BIN.post('/deviceData', unique_id);
}

export function readSensorData(): AxiosPromise<SensorData> {
  return AXIOS.get('/sensorData');
}

export function readDeviceEntities(unique_id: UniqueID): AxiosPromise<DeviceEntity[]> {
  return AXIOS_BIN.post('/deviceEntities', unique_id);
}

export function writeExcludeEntities(excludeEntities: ExcludeEntities): AxiosPromise<void> {
  return AXIOS.post('/excludeEntities', excludeEntities);
}

export function writeValue(writevalue: WriteValue): AxiosPromise<void> {
  return AXIOS.post('/writeValue', writevalue);
}

export function writeSensor(writesensor: WriteSensor): AxiosPromise<void> {
  return AXIOS.post('/writeSensor', writesensor);
}

export function writeAnalog(writeanalog: WriteAnalog): AxiosPromise<void> {
  return AXIOS.post('/writeAnalog', writeanalog);
}

// EMS-ESP API calls
export function API(apiCall: APIcall): AxiosPromise<void> {
  return AXIOS_API.post('/', apiCall);
}
