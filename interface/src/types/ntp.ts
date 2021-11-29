export enum NTPSyncStatus {
  NTP_INACTIVE = 0,
  NTP_ACTIVE = 1
}

export interface NTPStatus {
  status: NTPSyncStatus;
  utc_time: string;
  local_time: string;
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
  local_time: string;
}
