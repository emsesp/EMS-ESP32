export enum NTPSyncStatus {
  NTP_INACTIVE = 0,
  NTP_ACTIVE = 1
}

export interface NTPStatus {
  status: NTPSyncStatus;
  time_utc: string;
  time_local: string;
  server: string;
  uptime: number;
}

export interface NTPSettings {
  enabled: boolean;
  server: string;
  tz_label: string;
  tz_format: string;
}

export interface Time {
  time_utc: string;
}
