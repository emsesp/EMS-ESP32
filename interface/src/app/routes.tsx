import type { ComponentType, ReactNode } from 'react';
import { Navigate, Route } from 'react-router';

import Commands from 'app/main/Commands';
import CustomEntities from 'app/main/CustomEntities';
import Customizations from 'app/main/Customizations';
import Dashboard from 'app/main/Dashboard';
import Devices from 'app/main/Devices';
import Help from 'app/main/Help';
import Modules from 'app/main/Modules';
import Scheduler from 'app/main/Scheduler';
import Sensors from 'app/main/Sensors';
import UserProfile from 'app/main/UserProfile';
import APSettings from 'app/settings/APSettings';
import ApplicationSettings from 'app/settings/ApplicationSettings';
import DownloadUpload from 'app/settings/DownloadUpload';
import HeatingRestore from 'app/settings/HeatingRestore';
import MqttSettings from 'app/settings/MqttSettings';
import NTPSettings from 'app/settings/NTPSettings';
import Settings from 'app/settings/Settings';
import Version from 'app/settings/Version';
import Network from 'app/settings/network/Network';
import NetworkSettings from 'app/settings/network/NetworkSettings';
import WiFiNetworkScanner from 'app/settings/network/WiFiNetworkScanner';
import ApiKeys from 'app/settings/security/ApiKeys';
import ManageUsers from 'app/settings/security/ManageUsers';
import Security from 'app/settings/security/Security';
import APStatus from 'app/status/APStatus';
import Activity from 'app/status/Activity';
import HardwareStatus from 'app/status/HardwareStatus';
import MqttStatus from 'app/status/MqttStatus';
import NTPStatus from 'app/status/NTPStatus';
import NetworkStatus from 'app/status/NetworkStatus';
import Status from 'app/status/Status';
import SystemLog from 'app/status/SystemLog';
import type { IconName } from 'components/ui';
import type { TranslationFunctions } from 'i18n/i18n-types';
import { routeMatches } from 'utils';

export type MenuGroup = 'main' | 'modules' | 'system' | 'account';

export interface AppRoute {
  // absolute URL path; never change one, bookmarks and firmware redirects use them
  path: string;
  element: ComponentType;
  title: (LL: TranslationFunctions) => string;
  icon: IconName;
  // path of the route one level up, for breadcrumbs
  parent?: string;
  admin?: boolean;
  badge?: 'update';
  // drawer section; routes without one are not in the drawer. Order follows this table.
  menu?: MenuGroup;
  // also render for sub-paths (path/*)
  wildcard?: boolean;
  // nested <Route>s, e.g. tabs
  children?: ReactNode;
}

// prettier-ignore
export const routes: AppRoute[] = [
  { path: '/dashboard', element: Dashboard, title: (LL) => LL.DASHBOARD(), icon: 'star', menu: 'main', wildcard: true },
  { path: '/devices', element: Devices, title: (LL) => LL.DEVICES(), icon: 'category', menu: 'main', wildcard: true },

  { path: '/sensors', element: Sensors, title: (LL) => LL.SENSORS(), icon: 'sensors', menu: 'modules', wildcard: true },
  { path: '/customizations', element: Customizations, title: (LL) => LL.CUSTOMIZATIONS(), icon: 'construction', menu: 'modules', admin: true },
  { path: '/commands', element: Commands, title: (LL) => LL.COMMANDS(), icon: 'playlist_play', menu: 'modules', admin: true },
  { path: '/scheduler', element: Scheduler, title: (LL) => LL.SCHEDULER(), icon: 'more_time', menu: 'modules', admin: true },
  { path: '/customentities', element: CustomEntities, title: (LL) => LL.CUSTOM_ENTITIES(0), icon: 'playlist_add', menu: 'modules', admin: true },

  { path: '/status', element: Status, title: (LL) => LL.STATUS(), icon: 'assessment', menu: 'system', wildcard: true },
  { path: '/status/hardwarestatus', element: HardwareStatus, title: (LL) => LL.HARDWARE(), icon: 'memory', parent: '/status', wildcard: true },
  { path: '/status/activity', element: Activity, title: (LL) => LL.DATA_TRAFFIC(), icon: 'swap_horiz', parent: '/status' },
  { path: '/status/network', element: NetworkStatus, title: (LL) => LL.NETWORK(1), icon: 'lan', parent: '/status' },
  { path: '/status/mqtt', element: MqttStatus, title: () => 'MQTT', icon: 'device_hub', parent: '/status' },
  { path: '/status/ntp', element: NTPStatus, title: () => 'NTP', icon: 'schedule', parent: '/status' },
  { path: '/status/ap', element: APStatus, title: (LL) => LL.ACCESS_POINT(0), icon: 'wifi_tethering', parent: '/status' },
  { path: '/status/log', element: SystemLog, title: (LL) => LL.SYSTEM_LOG(), icon: 'list_alt', parent: '/status' },
  // old address of the firmware page, kept for bookmarks
  { path: '/status/version', element: () => <Navigate replace to="/settings/version" />, title: () => 'EMS-ESP Firmware', icon: 'new_releases', parent: '/settings' },

  { path: '/settings', element: Settings, title: (LL) => LL.SETTINGS(0), icon: 'settings', menu: 'system', badge: 'update' },
  { path: '/settings/application', element: ApplicationSettings, title: (LL) => LL.APPLICATION(), icon: 'settings_applications', parent: '/settings', admin: true },
  {
    path: '/settings/network',
    element: Network,
    title: (LL) => LL.NETWORK(0),
    icon: 'settings_ethernet',
    parent: '/settings',
    admin: true,
    children: (
      <>
        <Route index element={<Navigate replace to="/settings/network/settings" />} />
        <Route path="settings" element={<NetworkSettings />} />
        <Route path="scan" element={<WiFiNetworkScanner />} />
        <Route path="*" element={<Navigate replace to="/settings/network/settings" />} />
      </>
    )
  },
  { path: '/settings/ap', element: APSettings, title: (LL) => LL.ACCESS_POINT(0), icon: 'wifi_tethering', parent: '/settings', admin: true },
  { path: '/settings/ntp', element: NTPSettings, title: () => 'NTP', icon: 'schedule', parent: '/settings', admin: true },
  { path: '/settings/mqtt', element: MqttSettings, title: () => 'MQTT', icon: 'device_hub', parent: '/settings', admin: true },
  {
    path: '/settings/security',
    element: Security,
    title: (LL) => LL.SECURITY(0),
    icon: 'lock',
    parent: '/settings',
    admin: true,
    children: (
      <>
        <Route index element={<Navigate replace to="/settings/security/users" />} />
        <Route path="users" element={<ManageUsers />} />
        <Route path="apikeys" element={<ApiKeys />} />
        <Route path="*" element={<Navigate replace to="/settings/security/users" />} />
      </>
    )
  },
  { path: '/settings/modules', element: Modules, title: (LL) => LL.MODULES(), icon: 'extension', parent: '/settings', admin: true },
  { path: '/settings/downloadUpload', element: DownloadUpload, title: (LL) => LL.DOWNLOAD_UPLOAD(), icon: 'import_export', parent: '/settings', admin: true },
  { path: '/settings/downloadUpload/restore', element: HeatingRestore, title: (LL) => LL.HB_RESTORE(), icon: 'settings_backup_restore', parent: '/settings/downloadUpload', admin: true },
  { path: '/settings/version', element: Version, title: () => 'EMS-ESP Firmware', icon: 'new_releases', parent: '/settings' },

  { path: '/help', element: Help, title: (LL) => LL.HELP(), icon: 'live_help', menu: 'system', wildcard: true },
  { path: '/user', element: UserProfile, title: (LL) => LL.USER_PROFILE(), icon: 'account_circle', menu: 'account', wildcard: true }
];

export const renderRoute = ({
  path,
  element: Element,
  wildcard,
  children
}: AppRoute) => (
  <Route
    key={path}
    path={path.slice(1) + (wildcard ? '/*' : '')}
    element={<Element />}
  >
    {children}
  </Route>
);

// the most specific route for a pathname, e.g. /settings/network for /settings/network/scan
export const findRoute = (pathname: string) =>
  routes
    .filter((r) => routeMatches(r.path, pathname))
    .reduce<AppRoute | undefined>(
      (best, r) => (!best || r.path.length > best.path.length ? r : best),
      undefined
    );

// breadcrumb trail from the top-level route down to the current one
export const routeTrail = (pathname: string): AppRoute[] => {
  const trail: AppRoute[] = [];
  let route = findRoute(pathname);
  while (route) {
    trail.unshift(route);
    const { parent } = route;
    route = parent ? routes.find((r) => r.path === parent) : undefined;
  }
  return trail;
};

export const menuRoutes = (group: MenuGroup) =>
  routes.filter((r) => r.menu === group);
