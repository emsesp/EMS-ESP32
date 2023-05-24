import type {
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
  CustomEntities,
  WriteDeviceValue,
  WriteTemperatureSensor,
  WriteAnalogSensor,
  SensorData,
  Schedule,
  Entities
} from './types';
import type { AxiosPromise } from 'axios';
import { AXIOS, AXIOS_API, AXIOS_BIN } from 'api/endpoints';

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

export function writeCustomEntities(customEntities: CustomEntities): AxiosPromise<void> {
  return AXIOS.post('/customEntities', customEntities);
}

export function writeDeviceValue(dv: WriteDeviceValue): AxiosPromise<void> {
  return AXIOS.post('/writeDeviceValue', dv);
}

export function writeTemperatureSensor(ts: WriteTemperatureSensor): AxiosPromise<void> {
  return AXIOS.post('/writeTemperatureSensor', ts);
}

export function writeAnalogSensor(as: WriteAnalogSensor): AxiosPromise<void> {
  return AXIOS.post('/writeAnalogSensor', as);
}

export function resetCustomizations(): AxiosPromise<void> {
  return AXIOS.post('/resetCustomizations');
}

export function API(apiCall: APIcall): AxiosPromise<void> {
  return AXIOS_API.post('/', apiCall);
}

export function getSettings(): AxiosPromise<void> {
  return AXIOS.get('/getSettings');
}

export function getCustomizations(): AxiosPromise<void> {
  return AXIOS.get('/getCustomizations');
}

export function getSchedule(): AxiosPromise<Schedule> {
  return AXIOS.get('/getSchedule');
}

export function readSchedule(): AxiosPromise<Schedule> {
  return AXIOS.get('/schedule');
}

export function writeSchedule(schedule: Schedule): AxiosPromise<void> {
  return AXIOS.post('/schedule', schedule);
}

export function getEntities(): AxiosPromise<Entities> {
  return AXIOS.get('/getEntities');
}

export function readEntities(): AxiosPromise<Entities> {
  return AXIOS.get('/entities');
}

export function writeEntities(entities: Entities): AxiosPromise<void> {
  return AXIOS.post('/entities', entities);
}
