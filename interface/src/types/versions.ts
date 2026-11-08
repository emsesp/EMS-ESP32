// Types for the `getVersions` action response coming from the device.
// The device proxies the request to emsesp.org/versions.json. If the device
// is offline the `stable` and `dev` fields are omitted, but `system_name` is
// always sent as it comes from the device itself.

export interface VersionInfo {
  version: string;
  date: string;
}

export interface RemoteVersionInfo extends VersionInfo {
  upgradeable: boolean;
}

export interface CurrentVersionInfo extends VersionInfo {
  type: 'stable' | 'dev';
  upgradeable: boolean;
}

export interface VersionsResponse {
  current: CurrentVersionInfo;
  stable?: RemoteVersionInfo;
  dev?: RemoteVersionInfo;
  system_name?: string;
}
