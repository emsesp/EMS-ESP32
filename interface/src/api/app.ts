import { alovaInstance } from 'api/endpoints';

import type {
  APIcall,
  Action,
  Activity,
  CommandItem,
  Commands,
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

const MSGPACK_CONFIG = { responseType: 'arraybuffer' as const };

// Dashboard
export const readDashboard = () =>
  alovaInstance.Get<DashboardData>('/rest/dashboardData', MSGPACK_CONFIG);

// Devices
export const readCoreData = () => alovaInstance.Get<CoreData>('/rest/coreData');
export const readDeviceData = (id: number) =>
  alovaInstance.Get<DeviceData>('/rest/deviceData', {
    params: { id },
    ...MSGPACK_CONFIG
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
  alovaInstance.Get<DeviceEntity[]>('/rest/deviceEntities', {
    params: { id },
    ...MSGPACK_CONFIG
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
      return [...(data as Schedule).schedule];
    }
  });
export const writeSchedule = (data: Schedule) =>
  alovaInstance.Post('/rest/schedule', data);

// Commands
export const readCommands = () =>
  alovaInstance.Get<CommandItem[]>('/rest/commands', {
    transform(data) {
      const commands = (data as Commands).commands;
      // commands saved before the guest flag existed
      return commands.map((ci) => ({ ...ci, guest: ci.guest ?? true }));
    }
  });
export const writeCommands = (data: Commands) =>
  alovaInstance.Post('/rest/commands', data);

// Modules
export const readModules = () =>
  alovaInstance.Get<ModuleItem[]>('/rest/modules', {
    transform(data) {
      return [...(data as Modules).modules];
    }
  });
export const writeModules = (data: {
  modules: { key: string; enabled: boolean; license: string }[];
}) => alovaInstance.Post('/rest/modules', data);

// CustomEntities
export const readCustomEntities = () =>
  alovaInstance.Get<EntityItem[]>('/rest/customEntities', {
    transform(data) {
      return [...(data as Entities).entities];
    }
  });
export const writeCustomEntities = (data: Entities) =>
  alovaInstance.Post('/rest/customEntities', data);

// What the user moved between Normal and All values in the Dashboard menu: the key of a value -> its level
export type ViewLevels = Record<string, 1 | 2>;
export const readViewLevels = () =>
  alovaInstance.Get<ViewLevels>('/rest/viewLevels');
export const writeViewLevels = (data: ViewLevels) =>
  alovaInstance.Post('/rest/viewLevels', data);
