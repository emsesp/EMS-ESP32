import { FC } from 'react';

import { CgSmartHomeBoiler } from 'react-icons/cg';
import { MdOutlineSensors } from 'react-icons/md';
import { FaSolarPanel } from 'react-icons/fa';
import { MdThermostatAuto } from 'react-icons/md';
import { AiOutlineControl } from 'react-icons/ai';
import { GiHeatHaze } from 'react-icons/gi';
import { TiFlowSwitch } from 'react-icons/ti';
import { VscVmConnect } from 'react-icons/vsc';
import { AiOutlineGateway } from 'react-icons/ai';
import { AiOutlineAlert } from 'react-icons/ai';
import { AiOutlineChrome } from 'react-icons/ai';

interface DeviceIconProps {
  type: string;
}

const DeviceIcon: FC<DeviceIconProps> = ({ type }) => {
  switch (type) {
    case 'Boiler':
      return <CgSmartHomeBoiler />;
    case 'Sensor':
      return <MdOutlineSensors />;
    case 'Solar':
      return <FaSolarPanel />;
    case 'Thermostat':
      return <MdThermostatAuto />;
    case 'Mixer':
      return <AiOutlineControl />;
    case 'Heatpump':
      return <GiHeatHaze />;
    case 'Switch':
      return <TiFlowSwitch />;
    case 'Connect':
      return <VscVmConnect />;
    case 'Gateway':
      return <AiOutlineGateway />;
    case 'Alert':
      return <AiOutlineAlert />;
    case 'Pump':
      return <AiOutlineChrome />;
    default:
      return null;
  }
};

export default DeviceIcon;
