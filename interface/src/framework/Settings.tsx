import { type FC, useState } from 'react';

import AccessTimeIcon from '@mui/icons-material/AccessTime';
import CancelIcon from '@mui/icons-material/Cancel';
import DeviceHubIcon from '@mui/icons-material/DeviceHub';
import ImportExportIcon from '@mui/icons-material/ImportExport';
import LockIcon from '@mui/icons-material/Lock';
import MemoryIcon from '@mui/icons-material/Memory';
import SettingsBackupRestoreIcon from '@mui/icons-material/SettingsBackupRestore';
import SettingsEthernetIcon from '@mui/icons-material/SettingsEthernet';
import SettingsInputAntennaIcon from '@mui/icons-material/SettingsInputAntenna';
import TuneIcon from '@mui/icons-material/Tune';
import {
  Box,
  Button,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  Divider,
  List
} from '@mui/material';

import * as SystemApi from 'api/system';

import { dialogStyle } from 'CustomTheme';
import { useRequest } from 'alova';
import { SectionContent, useLayoutTitle } from 'components';
import ListMenuItem from 'components/layout/ListMenuItem';
import { useI18nContext } from 'i18n/i18n-react';

const Settings: FC = () => {
  const { LL } = useI18nContext();
  useLayoutTitle(LL.SETTINGS(0));

  const [confirmFactoryReset, setConfirmFactoryReset] = useState<boolean>(false);

  const { send: factoryResetCommand } = useRequest(SystemApi.factoryReset(), {
    immediate: false
  });

  const factoryReset = async () => {
    await factoryResetCommand();
    setConfirmFactoryReset(false);
  };

  const renderFactoryResetDialog = () => (
    <Dialog
      sx={dialogStyle}
      open={confirmFactoryReset}
      onClose={() => setConfirmFactoryReset(false)}
    >
      <DialogTitle>{LL.FACTORY_RESET()}</DialogTitle>
      <DialogContent dividers>{LL.SYSTEM_FACTORY_TEXT_DIALOG()}</DialogContent>
      <DialogActions>
        <Button
          startIcon={<CancelIcon />}
          variant="outlined"
          onClick={() => setConfirmFactoryReset(false)}
          color="secondary"
        >
          {LL.CANCEL()}
        </Button>
        <Button
          startIcon={<SettingsBackupRestoreIcon />}
          variant="outlined"
          onClick={factoryReset}
          color="error"
        >
          {LL.FACTORY_RESET()}
        </Button>
      </DialogActions>
    </Dialog>
  );

  const content = () => (
    <>
      <List sx={{ borderRadius: 3, border: '2px solid grey' }}>
        <ListMenuItem
          icon={TuneIcon}
          bgcolor="#134ba2"
          label={LL.APPLICATION()}
          text={LL.APPLICATION_SETTINGS_1()}
          to="ems-esp"
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

        <Divider />

        <ListMenuItem
          icon={ImportExportIcon}
          bgcolor="#5d89f7"
          label={LL.UPLOAD_DOWNLOAD()}
          text={LL.UPLOAD_DOWNLOAD_1()}
          to="upload"
        />
      </List>

      {renderFactoryResetDialog()}

      <Box mt={2} display="flex" flexWrap="wrap">
        <Button
          startIcon={<SettingsBackupRestoreIcon />}
          variant="outlined"
          onClick={() => setConfirmFactoryReset(true)}
          color="error"
        >
          {LL.FACTORY_RESET()}
        </Button>
      </Box>
    </>
  );

  return <SectionContent>{content()}</SectionContent>;
};

export default Settings;
