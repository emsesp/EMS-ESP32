export enum EspPlatform {
  ESP8266 = "esp8266",
  ESP32 = "esp32"
}

interface ESPSystemStatus {
  esp_platform: EspPlatform;
  max_alloc_heap: number;
  cpu_freq_mhz: number;
  free_heap: number;
  sketch_size: number;
  free_sketch_space: number;
  sdk_version: string;
  flash_chip_size: number;
  flash_chip_speed: number;
  fs_used: number;
  fs_total: number;
}

export interface ESP32SystemStatus extends ESPSystemStatus {
  esp_platform: EspPlatform.ESP32;
  psram_size: number;
  free_psram: number;
}

export interface ESP8266SystemStatus extends ESPSystemStatus {
  esp_platform: EspPlatform.ESP8266;
  heap_fragmentation: number;
}

export type SystemStatus = ESP8266SystemStatus | ESP32SystemStatus;

export interface OTASettings {
  enabled: boolean;
  port: number;
  password: string;
}
