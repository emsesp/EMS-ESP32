import { FC } from 'react';

import { CgSmartHomeBoiler } from 'react-icons/cg';
import { FaSolarPanel } from 'react-icons/fa';
import { MdThermostatAuto, MdOutlineSensors } from 'react-icons/md';
import { GiHeatHaze } from 'react-icons/gi';
import { TiFlowSwitch } from 'react-icons/ti';
import { VscVmConnect } from 'react-icons/vsc';
import { AiOutlineControl, AiOutlineGateway, AiOutlineAlert, AiOutlineChrome } from 'react-icons/ai';

interface DeviceIconProps {
  type_id: number;
}

// matches emsdevice.h DeviceType
const enum DeviceType {
  SYSTEM = 0,
  DALLASSENSOR,
  ANALOGSENSOR,
  SCHEDULER,
  BOILER,
  THERMOSTAT,
  MIXER,
  SOLAR,
  HEATPUMP,
  GATEWAY,
  SWITCH,
  CONTROLLER,
  CONNECT,
  ALERT,
  PUMP,
  GENERIC,
  HEATSOURCE,
  UNKNOWN
}

const DeviceIcon: FC<DeviceIconProps> = ({ type_id }) => {
  switch (type_id) {
    case DeviceType.DALLASSENSOR:
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
    case DeviceType.PUMP:
      return <AiOutlineChrome />;
    default:
      return null;
  }
};

export default DeviceIcon;
