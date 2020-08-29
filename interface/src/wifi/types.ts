export enum WiFiConnectionStatus {
  WIFI_STATUS_IDLE = 0,
  WIFI_STATUS_NO_SSID_AVAIL = 1,
  WIFI_STATUS_CONNECTED = 3,
  WIFI_STATUS_CONNECT_FAILED = 4,
  WIFI_STATUS_CONNECTION_LOST = 5,
  WIFI_STATUS_DISCONNECTED = 6,
  WIFI_STATUS_NO_SHIELD = 255
}

export enum WiFiEncryptionType {
  WIFI_AUTH_OPEN = 0,
  WIFI_AUTH_WEP = 1,
  WIFI_AUTH_WEP_PSK = 2,
  WIFI_AUTH_WEP2_PSK = 3,
  WIFI_AUTH_WPA_WPA2_PSK = 4,
  WIFI_AUTH_WPA2_ENTERPRISE = 5
}

export interface WiFiStatus {
  status: WiFiConnectionStatus;
  local_ip: string;
  mac_address: string;
  rssi: number;
  ssid: string;
  bssid: string;
  channel: number;
  subnet_mask: string;
  gateway_ip: string;
  dns_ip_1: string;
  dns_ip_2: string;
}

export interface WiFiSettings {
  ssid: string;
  password: string;
  hostname: string;
  static_ip_config: boolean;
  local_ip?: string;
  gateway_ip?: string;
  subnet_mask?: string;
  dns_ip_1?: string;
  dns_ip_2?: string;
}

export interface WiFiNetworkList {
  networks: WiFiNetwork[];
}

export interface WiFiNetwork {
  rssi: number;
  ssid: string;
  bssid: string;
  channel: number;
  encryption_type: WiFiEncryptionType;
}
