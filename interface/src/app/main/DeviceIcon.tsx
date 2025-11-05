import { memo } from 'react';
import { AiOutlineAlert, AiOutlineControl, AiOutlineGateway } from 'react-icons/ai';
import { CgSmartHomeBoiler } from 'react-icons/cg';
import { FaSolarPanel } from 'react-icons/fa';
import { GiHeatHaze, GiTap } from 'react-icons/gi';
import {
  MdMoreTime,
  MdOutlineDevices,
  MdOutlinePool,
  MdOutlineSensors,
  MdPlaylistAdd,
  MdThermostatAuto
} from 'react-icons/md';
import { PiFan, PiGauge } from 'react-icons/pi';
import { TiFlowSwitch, TiThermometer } from 'react-icons/ti';
import { VscVmConnect } from 'react-icons/vsc';

import type { SvgIconProps } from '@mui/material';

import { DeviceType } from './types';

const deviceIconLookup: Record<
  DeviceType,
  React.ComponentType<SvgIconProps> | null
> = {
  [DeviceType.TEMPERATURESENSOR]: TiThermometer,
  [DeviceType.ANALOGSENSOR]: PiGauge,
  [DeviceType.BOILER]: CgSmartHomeBoiler,
  [DeviceType.HEATSOURCE]: CgSmartHomeBoiler,
  [DeviceType.THERMOSTAT]: MdThermostatAuto,
  [DeviceType.MIXER]: AiOutlineControl,
  [DeviceType.SOLAR]: FaSolarPanel,
  [DeviceType.HEATPUMP]: GiHeatHaze,
  [DeviceType.GATEWAY]: AiOutlineGateway,
  [DeviceType.SWITCH]: TiFlowSwitch,
  [DeviceType.CONTROLLER]: VscVmConnect,
  [DeviceType.CONNECT]: VscVmConnect,
  [DeviceType.ALERT]: AiOutlineAlert,
  [DeviceType.EXTENSION]: MdOutlineDevices,
  [DeviceType.WATER]: GiTap,
  [DeviceType.POOL]: MdOutlinePool,
  [DeviceType.CUSTOM]: MdPlaylistAdd,
  [DeviceType.UNKNOWN]: MdOutlineSensors,
  [DeviceType.SYSTEM]: null,
  [DeviceType.SCHEDULER]: MdMoreTime,
  [DeviceType.GENERIC]: MdOutlineSensors,
  [DeviceType.VENTILATION]: PiFan
};

interface DeviceIconProps {
  type_id: DeviceType;
}

const DeviceIcon = memo(({ type_id }: DeviceIconProps) => {
  const Icon = deviceIconLookup[type_id];
  return Icon ? <Icon /> : null;
});

export default DeviceIcon;
