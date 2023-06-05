import { unpack } from '../api/unpack';
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
  WriteTemperatureSensor,
  WriteAnalogSensor,
  SensorData,
  Schedule,
  Entities,
  DeviceValue
} from './types';
import type { AxiosPromise } from 'axios';
import { AXIOS, AXIOS_API, AXIOS_BIN, alovaInstance } from 'api/endpoints';

export const readCoreData = () => alovaInstance.Get<CoreData>(`/coreData`);

// uses msgpack
export const readDeviceData = (id: number) =>
  alovaInstance.Get<DeviceData>('/deviceData', {
    params: { id },
    responseType: 'arraybuffer',
    transformData(data) {
      return unpack(data);
    }
  });

export const writeDeviceValue = (id: number, devicevalue: DeviceValue) =>
  alovaInstance.Post('/writeDeviceValue', { id, devicevalue });

//
// TODO change below to use alova
//

export function restart(): AxiosPromise<void> {
  return AXIOS.post('/restart');
}

export function readSettings(): AxiosPromise<Settings> {
  return AXIOS.get('/settings');
}

export function writeSettings(settings: Settings): AxiosPromise<Settings> {
  return AXIOS.post('/settings', settings); // call command
}

// TODO change to GET
export function getBoardProfile(boardProfile: BoardProfileName): AxiosPromise<BoardProfile> {
  return AXIOS.post('/boardProfile', boardProfile);
}

export function readStatus(): AxiosPromise<Status> {
  return AXIOS.get('/status');
}

export function readDevices(): AxiosPromise<Devices> {
  return AXIOS.get('/devices');
}

export function scanDevices(): AxiosPromise<void> {
  return AXIOS.post('/scanDevices'); // call command
}

export function readSensorData(): AxiosPromise<SensorData> {
  return AXIOS.get('/sensorData');
}

// TODO change to GET
export function readDeviceEntities(unique_id: UniqueID): AxiosPromise<DeviceEntity[]> {
  return AXIOS_BIN.post('/deviceEntities', unique_id);
}

export function writeCustomEntities(customEntities: CustomEntities): AxiosPromise<void> {
  return AXIOS.post('/customEntities', customEntities);
}

export function writeTemperatureSensor(ts: WriteTemperatureSensor): AxiosPromise<void> {
  return AXIOS.post('/writeTemperatureSensor', ts);
}

export function writeAnalogSensor(as: WriteAnalogSensor): AxiosPromise<void> {
  return AXIOS.post('/writeAnalogSensor', as);
}

export function resetCustomizations(): AxiosPromise<void> {
  return AXIOS.post('/resetCustomizations'); // command
}

export function API(apiCall: APIcall): AxiosPromise<void> {
  return AXIOS_API.post('/', apiCall); // command
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
