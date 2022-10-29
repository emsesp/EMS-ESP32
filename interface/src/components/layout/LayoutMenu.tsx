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

import { useI18nContext } from '../../i18n/i18n-react';

const LayoutMenu: FC = () => {
  const { features } = useContext(FeaturesContext);
  const authenticatedContext = useContext(AuthenticatedContext);
  const { LL } = useI18nContext();

  return (
    <>
      {features.project && (
        <List disablePadding component="nav">
          <ProjectMenu />
          <Divider />
        </List>
      )}
      <List disablePadding component="nav">
        <LayoutMenuItem icon={SettingsEthernetIcon} label={LL.NETWORK(0)} to="/network" />
        <LayoutMenuItem icon={SettingsInputAntennaIcon} label={LL.ACCESS_POINT(0)} to="/ap" />
        {features.ntp && <LayoutMenuItem icon={AccessTimeIcon} label="NTP" to="/ntp" />}
        {features.mqtt && <LayoutMenuItem icon={DeviceHubIcon} label="MQTT" to="/mqtt" />}
        <LayoutMenuItem
          icon={LockIcon}
          label={LL.SECURITY(0)}
          to="/security"
          disabled={!authenticatedContext.me.admin}
        />
        <LayoutMenuItem icon={SettingsIcon} label={LL.SYSTEM(0)} to="/system" />
      </List>
    </>
  );
};

export default LayoutMenu;
