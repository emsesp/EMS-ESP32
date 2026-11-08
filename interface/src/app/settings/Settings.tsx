import { useContext } from 'react';

import AccessTimeIcon from '@mui/icons-material/AccessTime';
import BuildIcon from '@mui/icons-material/Build';
import DeviceHubIcon from '@mui/icons-material/DeviceHub';
import ImportExportIcon from '@mui/icons-material/ImportExport';
import LockIcon from '@mui/icons-material/Lock';
import SettingsEthernetIcon from '@mui/icons-material/SettingsEthernet';
import SettingsInputAntennaIcon from '@mui/icons-material/SettingsInputAntenna';
import TuneIcon from '@mui/icons-material/Tune';
import ViewModuleIcon from '@mui/icons-material/ViewModule';
import { List } from '@mui/material';

import { SectionContent, useLayoutTitle } from 'components';
import ListMenuItem from 'components/layout/ListMenuItem';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';

const Settings = () => {
  const { LL } = useI18nContext();
  const { versions } = useContext(AuthenticatedContext);
  useLayoutTitle(LL.SETTINGS(0));

  const upgradeAvailable = versions?.current?.upgradeable ?? false;
  const firmwareText = versions?.current?.version
    ? `v${versions.current.version}${upgradeAvailable ? ` (${LL.UPDATE_AVAILABLE()})` : ''}`
    : '';

  return (
    <SectionContent>
      <List>
        <ListMenuItem
          icon={BuildIcon}
          bgcolor="#72caf9"
          label="EMS-ESP Firmware"
          text={firmwareText}
          to="/settings/version"
          badge={upgradeAvailable}
        />

        <ListMenuItem
          icon={TuneIcon}
          bgcolor="#134ba2"
          label={LL.APPLICATION()}
          text={LL.APPLICATION_SETTINGS_1()}
          to="application"
        />

        <ListMenuItem
          icon={SettingsEthernetIcon}
          bgcolor="#40828f"
          label={LL.NETWORK(0)}
          text={LL.CONFIGURE(LL.SETTINGS_OF(LL.NETWORK(1)))}
          to="network"
        />

        <ListMenuItem
          icon={SettingsInputAntennaIcon}
          bgcolor="#5f9a5f"
          label={LL.ACCESS_POINT(0)}
          text={LL.CONFIGURE(LL.ACCESS_POINT(1))}
          to="ap"
        />

        <ListMenuItem
          icon={AccessTimeIcon}
          bgcolor="#c5572c"
          label="NTP"
          text={LL.CONFIGURE(LL.LOCAL_TIME(1))}
          to="ntp"
        />

        <ListMenuItem
          icon={DeviceHubIcon}
          bgcolor="#68374d"
          label="MQTT"
          text={LL.CONFIGURE('MQTT')}
          to="mqtt"
        />

        <ListMenuItem
          icon={LockIcon}
          label={LL.SECURITY(0)}
          text={LL.SECURITY_1()}
          to="security"
        />

        <ListMenuItem
          icon={ViewModuleIcon}
          bgcolor="#efc34b"
          label={LL.MODULES()}
          text={LL.MODULES_1()}
          to="modules"
        />

        <ListMenuItem
          icon={ImportExportIcon}
          bgcolor="#5d89f7"
          label={LL.DOWNLOAD_UPLOAD()}
          text={LL.DOWNLOAD_UPLOAD_1()}
          to="downloadUpload"
        />
      </List>
    </SectionContent>
  );
};

export default Settings;
