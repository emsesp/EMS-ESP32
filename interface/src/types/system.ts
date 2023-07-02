export interface SystemStatus {
  emsesp_version: string;
  esp_platform: string;
  max_alloc_heap: number;
  cpu_freq_mhz: number;
  free_heap: number;
  sdk_version: string;
  flash_chip_size: number;
  flash_chip_speed: number;
  app_used: number;
  app_free: number;
  fs_used: number;
  fs_free: number;
  uptime: string;
  free_mem: number;
  psram_size?: number;
  free_psram?: number;
  has_loader: boolean;
}

export interface OTASettings {
  enabled: boolean;
  port: number;
  password: string;
}

export enum LogLevel {
  ERROR = 3,
  WARNING = 4,
  NOTICE = 5,
  INFO = 6,
  DEBUG = 7,
  TRACE = 8,
  ALL = 9
}

export interface LogEntry {
  t: string;
  l: LogLevel;
  i: number;
  n: string;
  m: string;
}

export interface LogSettings {
  level: number;
  max_messages: number;
  compact: false;
}

export interface Version {
  version: string;
  url: string;
  changelog: string;
}
