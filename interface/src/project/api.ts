import type {
  APIcall,
  Settings,
  Status,
  CoreData,
  Devices,
  DeviceEntity,
  WriteTemperatureSensor,
  WriteAnalogSensor,
  SensorData,
  Schedule,
  Entities,
  DeviceData
} from './types';
import type { AxiosPromise } from 'axios';
import { AXIOS, AXIOS_API, alovaInstance } from 'api/endpoints';

// DashboardDevices
export const readCoreData = () => alovaInstance.Get<CoreData>(`/coreData`);
export const readDeviceData = (id: number) =>
  alovaInstance.Get<DeviceData>('/deviceData', {
    params: { id },
    responseType: 'arraybuffer' // uses msgpack
  });
export const writeDeviceValue = (data: any) => alovaInstance.Post('/writeDeviceValue', data);

// SettingsApplication
export const readSettings = () => alovaInstance.Get<Settings>('/settings');
export const writeSettings = (data: any) => alovaInstance.Post('/settings', data);
export const getBoardProfile = (boardProfile: string) =>
  alovaInstance.Get('/boardProfile', {
    params: { boardProfile }
  });
export const restart = () => alovaInstance.Post('/restart');

// SettingsCustomization
export const readDeviceEntities = (id: number) =>
  alovaInstance.Get<DeviceEntity[]>('/deviceEntities', {
    params: { id },
    responseType: 'arraybuffer',
    transformData(rawData: any) {
      return rawData.map((de: DeviceEntity) => ({ ...de, o_m: de.m, o_cn: de.cn, o_mi: de.mi, o_ma: de.ma }));
    }
  });
export const readDevices = () => alovaInstance.Get<Devices>('/devices');
export const resetCustomizations = () => alovaInstance.Post('/resetCustomizations');
export const writeCustomEntities = (data: any) => alovaInstance.Post('/customEntities', data);

// DashboardSensors
export const readSensorData = () => alovaInstance.Get<SensorData>('/sensorData');
export const writeTemperatureSensor = (ts: WriteTemperatureSensor) => alovaInstance.Post('/writeTemperatureSensor', ts);
export const writeAnalogSensor = (as: WriteAnalogSensor) => alovaInstance.Post('/writeAnalogSensor', as);

// TODO think about naming, get... and not get etc...

// DashboardStatus
export const readStatus = () => alovaInstance.Get<Status>('/status');
export const scanDevices = () => alovaInstance.Post('/scanDevices');

// ALOVA goes here....

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
