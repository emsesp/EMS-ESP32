export interface RecoveryPartition {
  label: string;
  factory: boolean;
  address: number;
  size: number;
  valid: boolean;
  image_size: number;
  version: string;
  install_date: number;
  // bootloader rollback state, empty if the partition has never been selected
  ota_state: '' | 'new' | 'pending' | 'valid' | 'invalid' | 'aborted';
}

export interface RecoveryStatus {
  recovery_version: string;
  authenticated: boolean;
  hostname: string;
  chip: string;
  flash_size: number;
  psram_size: number;
  free_heap: number;
  mac: string;
  uptime: number;
  reason: '' | 'crash' | 'request'; // why EMS-ESP switched to the recovery
  reset_reason:
    | 'power_on'
    | 'external'
    | 'software'
    | 'panic'
    | 'watchdog'
    | 'brownout'
    | 'other';
  ap_ssid: string;
  ap_ip: string;
  ap_clients: number;
  wifi_ssid: string;
  wifi_started: boolean; // false while waiting for Ethernet, or when Ethernet is connected
  wifi_connected: boolean;
  wifi_ip?: string;
  wifi_rssi?: number;
  static_ip: boolean;
  // only on boards with Ethernet configured
  eth_state?: 'disabled' | 'failed' | 'no_link' | 'connecting' | 'connected';
  eth_ip?: string;
  running: string;
  boot: string;
  upload_target: string;
  partitions: RecoveryPartition[];
}

export interface UploadResult {
  md5?: string;
  md5_ok?: boolean;
}
