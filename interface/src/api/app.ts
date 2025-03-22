import { alovaInstance } from 'api/endpoints';

import type {
  APIcall,
  Action,
  Activity,
  CoreData,
  DashboardData,
  DeviceData,
  DeviceEntity,
  Entities,
  EntityItem,
  ModuleItem,
  Modules,
  Schedule,
  ScheduleItem,
  SensorData,
  Settings,
  WriteAnalogSensor,
  WriteTemperatureSensor
} from '../app/main/types';

// Dashboard
export const readDashboard = () =>
  alovaInstance.Get<DashboardData>('/rest/dashboardData', {
    responseType: 'arraybuffer' // uses msgpack
  });

// Devices
export const readCoreData = () => alovaInstance.Get<CoreData>(`/rest/coreData`);
export const readDeviceData = (id: number) =>
  alovaInstance.Get<DeviceData>('/rest/deviceData', {
    // alovaInstance.Get<DeviceData>(`/rest/deviceData/${id}`, {
    params: { id },
    responseType: 'arraybuffer' // uses msgpack
  });
export const writeDeviceValue = (data: { id: number; c: string; v: unknown }) =>
  alovaInstance.Post('/rest/writeDeviceValue', data);

// Application Settings
export const readSettings = () => alovaInstance.Get<Settings>('/rest/settings');
export const writeSettings = (data: Settings) =>
  alovaInstance.Post('/rest/settings', data);
export const getBoardProfile = (boardProfile: string) =>
  alovaInstance.Get('/rest/boardProfile', {
    params: { boardProfile }
  });

// Sensors
export const readSensorData = () =>
  alovaInstance.Get<SensorData>('/rest/sensorData');
export const writeTemperatureSensor = (ts: WriteTemperatureSensor) =>
  alovaInstance.Post('/rest/writeTemperatureSensor', ts);
export const writeAnalogSensor = (as: WriteAnalogSensor) =>
  alovaInstance.Post('/rest/writeAnalogSensor', as);

// Activity
export const readActivity = () => alovaInstance.Get<Activity>('/rest/activity');

// API
export const API = (apiCall: APIcall) => alovaInstance.Post('/api', apiCall);

// Generic action
export const callAction = (action: Action) =>
  alovaInstance.Post('/rest/action', action);

// SettingsCustomization
export const readDeviceEntities = (id: number) =>
  // alovaInstance.Get<DeviceEntity[]>(`/rest/deviceEntities/${id}`, {
  alovaInstance.Get<DeviceEntity[]>(`/rest/deviceEntities`, {
    params: { id },
    responseType: 'arraybuffer',
    transform(data) {
      return (data as DeviceEntity[]).map((de: DeviceEntity) => ({
        ...de,
        o_m: de.m,
        o_cn: de.cn,
        o_mi: de.mi,
        o_ma: de.ma
      }));
    }
  });
export const resetCustomizations = () =>
  alovaInstance.Post('/rest/resetCustomizations');
export const writeCustomizationEntities = (data: {
  id: number;
  entity_ids: string[];
}) => alovaInstance.Post('/rest/customizationEntities', data);
export const writeDeviceName = (data: { id: number; name: string }) =>
  alovaInstance.Post('/rest/writeDeviceName', data);

// SettingsScheduler
export const readSchedule = () =>
  alovaInstance.Get<ScheduleItem[]>('/rest/schedule', {
    transform(data) {
      return (data as Schedule).schedule.map((si: ScheduleItem) => ({
        ...si,
        o_id: si.id,
        o_active: si.active,
        o_deleted: si.deleted,
        o_flags: si.flags,
        o_time: si.time,
        o_cmd: si.cmd,
        o_value: si.value,
        o_name: si.name
      }));
    }
  });
export const writeSchedule = (data: Schedule) =>
  alovaInstance.Post('/rest/schedule', data);

// Modules
export const readModules = () =>
  alovaInstance.Get<ModuleItem[]>('/rest/modules', {
    transform(data) {
      return (data as Modules).modules.map((mi: ModuleItem) => ({
        ...mi,
        o_enabled: mi.enabled,
        o_license: mi.license
      }));
    }
  });
export const writeModules = (data: {
  key: string;
  enabled: boolean;
  license: string;
}) => alovaInstance.Post('/rest/modules', data);

// CustomEntities
export const readCustomEntities = () =>
  alovaInstance.Get<EntityItem[]>('/rest/customEntities', {
    transform(data) {
      return (data as Entities).entities.map((ei: EntityItem) => ({
        ...ei,
        o_id: ei.id,
        o_ram: ei.ram,
        o_device_id: ei.device_id,
        o_type_id: ei.type_id,
        o_offset: ei.offset,
        o_factor: ei.factor,
        o_uom: ei.uom,
        o_value_type: ei.value_type,
        o_name: ei.name,
        o_writeable: ei.writeable,
        o_value: ei.value,
        o_deleted: ei.deleted
      }));
    }
  });
export const writeCustomEntities = (data: Entities) =>
  alovaInstance.Post('/rest/customEntities', data);
