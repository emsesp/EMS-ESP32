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
  type_id: number;
}

// matches emsdevice.h DeviceType
const DeviceIcon: FC<DeviceIconProps> = ({ type_id }) => {
  switch (type_id) {
    case 1:
    case 2:
      return <MdOutlineSensors />;
    case 3:
      return <CgSmartHomeBoiler />;
    case 4:
      return <MdThermostatAuto />;
    case 5:
      return <AiOutlineControl />;
    case 6:
      return <FaSolarPanel />;
    case 7:
      return <GiHeatHaze />;
    case 8:
      return <AiOutlineGateway />;
    case 9:
      return <TiFlowSwitch />;
    case 10:
    case 11:
      return <VscVmConnect />;
    case 12:
      return <AiOutlineAlert />;
    case 13:
      return <AiOutlineChrome />;
    default:
      return null;
  }
};

export default DeviceIcon;
