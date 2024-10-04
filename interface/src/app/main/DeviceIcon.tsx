import { AiOutlineAlert, AiOutlineControl, AiOutlineGateway } from 'react-icons/ai';
import { CgSmartHomeBoiler } from 'react-icons/cg';
import { FaSolarPanel } from 'react-icons/fa';
import { GiHeatHaze, GiTap } from 'react-icons/gi';
import { MdPlaylistAdd } from 'react-icons/md';
import {
  MdOutlineDevices,
  MdOutlinePool,
  MdOutlineSensors,
  MdThermostatAuto
} from 'react-icons/md';
import { TiFlowSwitch } from 'react-icons/ti';
import { VscVmConnect } from 'react-icons/vsc';

import type { SvgIconProps } from '@mui/material';

import { DeviceType } from './types';

const deviceIconLookup: {
  [key in DeviceType]: React.ComponentType<SvgIconProps> | undefined;
} = {
  [DeviceType.TEMPERATURESENSOR]: MdOutlineSensors,
  [DeviceType.ANALOGSENSOR]: MdOutlineSensors,
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
  [DeviceType.UNKNOWN]: undefined,
  [DeviceType.SYSTEM]: undefined,
  [DeviceType.SCHEDULER]: undefined,
  [DeviceType.GENERIC]: undefined,
  [DeviceType.VENTILATION]: undefined
};

const DeviceIcon = ({ type_id }: { type_id: DeviceType }) => {
  const Icon = deviceIconLookup[type_id];
  return Icon ? <Icon /> : null;
};

export default DeviceIcon;
