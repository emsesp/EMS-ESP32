import { alovaInstance } from 'api/endpoints';

// export type heatingbackup: the writeable values of each EMS device, keyed by command name (e.g. hc1/seltemp)
export type BackupValue = string | number | boolean;

export interface HeatingBackupDevice {
  type: string; // device type for the API, e.g. thermostat
  name: string;
  deviceId: number;
  productId: number;
  version: string;
  values: Record<string, BackupValue>;
}

export interface HeatingBackup {
  type: 'heatingbackup';
  version: string;
  // added by the Web UI when the file is saved
  created?: string;
  note?: string;
  fahrenheit: boolean;
  devices: HeatingBackupDevice[];
}

export const readHeatingBackup = () =>
  alovaInstance.Post<HeatingBackup>('/rest/action', {
    action: 'export',
    param: 'heatingbackup'
  });

export const isHeatingBackup = (data: unknown): data is HeatingBackup => {
  const b = data as HeatingBackup | null;
  return (
    !!b &&
    b.type === 'heatingbackup' &&
    Array.isArray(b.devices) &&
    b.devices.every(
      (d) => typeof d.type === 'string' && !!d.values && typeof d.values === 'object'
    )
  );
};

export const hex = (n: number) =>
  '0x' + n.toString(16).toUpperCase().padStart(2, '0');

// local time, as shown in the restore page
export const timestamp = (d = new Date()) => {
  const p = (n: number) => String(n).padStart(2, '0');
  return `${d.getFullYear()}-${p(d.getMonth() + 1)}-${p(d.getDate())} ${p(d.getHours())}:${p(d.getMinutes())}`;
};
