import { FC, useContext } from 'react';

import { Divider, List } from '@mui/material';

import SettingsInputAntennaIcon from '@mui/icons-material/SettingsInputAntenna';
import AccessTimeIcon from '@mui/icons-material/AccessTime';
import DeviceHubIcon from '@mui/icons-material/DeviceHub';
import SettingsIcon from '@mui/icons-material/Settings';
import LockIcon from '@mui/icons-material/Lock';
import SettingsEthernetIcon from '@mui/icons-material/SettingsEthernet';

import { FeaturesContext } from '../../contexts/features';
import ProjectMenu from '../../project/ProjectMenu';

import LayoutMenuItem from './LayoutMenuItem';
import { AuthenticatedContext } from '../../contexts/authentication';

const LayoutMenu: FC = () => {
  const { features } = useContext(FeaturesContext);
  const authenticatedContext = useContext(AuthenticatedContext);

  return (
    <>
      {features.project && (
        <List disablePadding component="nav">
          <ProjectMenu />
          <Divider />
        </List>
      )}
      <List disablePadding component="nav">
        <LayoutMenuItem icon={SettingsEthernetIcon} label="Network Connection" to="/network" />
        <LayoutMenuItem icon={SettingsInputAntennaIcon} label="Access Point" to="/ap" />
        {features.ntp && <LayoutMenuItem icon={AccessTimeIcon} label="Network Time" to="/ntp" />}
        {features.mqtt && <LayoutMenuItem icon={DeviceHubIcon} label="MQTT" to="/mqtt" />}
        <LayoutMenuItem icon={LockIcon} label="Security" to="/security" disabled={!authenticatedContext.me.admin} />
        <LayoutMenuItem icon={SettingsIcon} label="System" to="/system" />
      </List>
    </>
  );
};

export default LayoutMenu;
