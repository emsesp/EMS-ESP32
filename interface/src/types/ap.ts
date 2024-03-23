export enum APProvisionMode {
  AP_MODE_ALWAYS = 0,
  AP_MODE_DISCONNECTED = 1,
  AP_NEVER = 2
}

export enum APNetworkStatus {
  ACTIVE = 0,
  INACTIVE = 1,
  LINGERING = 2
}

export interface APStatusType {
  status: APNetworkStatus;
  ip_address: string;
  mac_address: string;
  station_num: number;
}

export interface APSettingsType {
  provision_mode: APProvisionMode;
  ssid: string;
  password: string;
  channel: number;
  ssid_hidden: boolean;
  max_clients: number;
  local_ip: string;
  gateway_ip: string;
  subnet_mask: string;
}
