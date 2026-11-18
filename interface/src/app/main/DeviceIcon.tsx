import { memo } from 'react';

import { Icon, type IconName } from 'components/ui';

import { DeviceType } from './types';

const deviceIconLookup: Record<DeviceType, IconName | null> = {
  [DeviceType.TEMPERATURESENSOR]: 'device_thermostat',
  [DeviceType.ANALOGSENSOR]: 'speed',
  [DeviceType.BOILER]: 'local_fire_department',
  [DeviceType.HEATSOURCE]: 'local_fire_department',
  [DeviceType.THERMOSTAT]: 'thermostat',
  [DeviceType.MIXER]: 'valve',
  [DeviceType.SOLAR]: 'solar_power',
  [DeviceType.HEATPUMP]: 'heat_pump',
  [DeviceType.GATEWAY]: 'router',
  [DeviceType.SWITCH]: 'toggle_on',
  [DeviceType.CONTROLLER]: 'settings_input_component',
  [DeviceType.CONNECT]: 'settings_input_component',
  [DeviceType.ALERT]: 'notification_important',
  [DeviceType.EXTENSION]: 'extension',
  [DeviceType.WATER]: 'water_drop',
  [DeviceType.POOL]: 'pool',
  [DeviceType.CUSTOM]: 'playlist_add',
  [DeviceType.UNKNOWN]: 'sensors',
  [DeviceType.SYSTEM]: null,
  [DeviceType.COMMAND]: 'playlist_add',
  [DeviceType.SCHEDULER]: 'more_time',
  [DeviceType.GENERIC]: 'sensors',
  [DeviceType.VENTILATION]: 'mode_fan'
};

interface DeviceIconProps {
  type_id: DeviceType;
  size?: 'sm' | 'xs';
  className?: string;
}

const DeviceIcon = memo(({ type_id, size, className }: DeviceIconProps) => {
  const name = deviceIconLookup[type_id];
  return name ? <Icon name={name} size={size} className={className} /> : null;
});

export default DeviceIcon;
