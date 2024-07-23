import type { FC } from 'react';
import { AiOutlineAlert, AiOutlineControl, AiOutlineGateway } from 'react-icons/ai';
import { CgSmartHomeBoiler } from 'react-icons/cg';
import { FaSolarPanel } from 'react-icons/fa';
import { GiHeatHaze, GiTap } from 'react-icons/gi';
import {
  MdOutlineDevices,
  MdOutlinePool,
  MdOutlineSensors,
  MdThermostatAuto
} from 'react-icons/md';
import { TiFlowSwitch } from 'react-icons/ti';
import { VscVmConnect } from 'react-icons/vsc';

import PlaylistAddIcon from '@mui/icons-material/PlaylistAdd';

import { DeviceType } from './types';

interface DeviceIconProps {
  type_id: number;
}

const DeviceIcon: FC<DeviceIconProps> = ({ type_id }) => {
  switch (type_id as DeviceType) {
    case DeviceType.TEMPERATURESENSOR:
    case DeviceType.ANALOGSENSOR:
      return <MdOutlineSensors />;
    case DeviceType.BOILER:
    case DeviceType.HEATSOURCE:
      return <CgSmartHomeBoiler />;
    case DeviceType.THERMOSTAT:
      return <MdThermostatAuto />;
    case DeviceType.MIXER:
      return <AiOutlineControl />;
    case DeviceType.SOLAR:
      return <FaSolarPanel />;
    case DeviceType.HEATPUMP:
      return <GiHeatHaze />;
    case DeviceType.GATEWAY:
      return <AiOutlineGateway />;
    case DeviceType.SWITCH:
      return <TiFlowSwitch />;
    case DeviceType.CONTROLLER:
    case DeviceType.CONNECT:
      return <VscVmConnect />;
    case DeviceType.ALERT:
      return <AiOutlineAlert />;
    case DeviceType.EXTENSION:
      return <MdOutlineDevices />;
    case DeviceType.WATER:
      return <GiTap />;
    case DeviceType.POOL:
      return <MdOutlinePool />;
    case DeviceType.CUSTOM:
      return (
        <PlaylistAddIcon
          sx={{ color: 'lightblue', fontSize: 22, verticalAlign: 'middle' }}
        />
      );
    default:
      return null;
  }
};

export default DeviceIcon;
