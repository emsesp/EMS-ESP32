import AccessTimeIcon from '@mui/icons-material/AccessTime';
import CancelIcon from '@mui/icons-material/Cancel';
import CastIcon from '@mui/icons-material/Cast';
import DeviceHubIcon from '@mui/icons-material/DeviceHub';
import ImportExportIcon from '@mui/icons-material/ImportExport';
import LockIcon from '@mui/icons-material/Lock';
import MemoryIcon from '@mui/icons-material/Memory';
import PowerSettingsNewIcon from '@mui/icons-material/PowerSettingsNew';
import SettingsBackupRestoreIcon from '@mui/icons-material/SettingsBackupRestore';
import SettingsEthernetIcon from '@mui/icons-material/SettingsEthernet';
import TuneIcon from '@mui/icons-material/Tune';

import { List, Button, Dialog, DialogActions, DialogContent, DialogTitle, Box } from '@mui/material';
import { useRequest } from 'alova';
import { useState, type FC } from 'react';
import { toast } from 'react-toastify';
import RestartMonitor from './system/RestartMonitor';
import { dialogStyle } from 'CustomTheme';
import * as SystemApi from 'api/system';
import { ButtonRow, SectionContent, useLayoutTitle } from 'components';
import ListMenuItem from 'components/layout/ListMenuItem';
import { useI18nContext } from 'i18n/i18n-react';

const Settings: FC = () => {
  const { LL } = useI18nContext();
  useLayoutTitle(LL.SETTINGS(0));

  const [confirmRestart, setConfirmRestart] = useState<boolean>(false);
  const [confirmFactoryReset, setConfirmFactoryReset] = useState<boolean>(false);
  const [processing, setProcessing] = useState<boolean>(false);
  const [restarting, setRestarting] = useState<boolean>();

  const { send: restartCommand } = useRequest(SystemApi.restart(), {
    immediate: false
  });

  const { send: factoryResetCommand } = useRequest(SystemApi.factoryReset(), {
    immediate: false
  });

  const { send: partitionCommand } = useRequest(SystemApi.partition(), {
    immediate: false
  });

  const restart = async () => {
    setProcessing(true);
    await restartCommand()
      .then(() => {
        setRestarting(true);
      })
      .catch((err) => {
        toast.error(err.message);
      })
      .finally(() => {
        setConfirmRestart(false);
        setProcessing(false);
      });
  };

  const factoryReset = async () => {
    setProcessing(true);
    await factoryResetCommand()
      .then(() => {
        setRestarting(true);
      })
      .catch((err) => {
        toast.error(err.message);
      })
      .finally(() => {
        setConfirmFactoryReset(false);
        setProcessing(false);
      });
  };

  const partition = async () => {
    setProcessing(true);
    await partitionCommand()
      .then(() => {
        setRestarting(true);
      })
      .catch((err) => {
        toast.error(err.message);
      })
      .finally(() => {
        setConfirmRestart(false);
        setProcessing(false);
      });
  };

  const renderRestartDialog = () => (
    <Dialog sx={dialogStyle} open={confirmRestart} onClose={() => setConfirmRestart(false)}>
      <DialogTitle>{LL.RESTART()}</DialogTitle>
      <DialogContent dividers>{LL.RESTART_CONFIRM()}</DialogContent>
      <DialogActions>
        <Button
          startIcon={<CancelIcon />}
          variant="outlined"
          onClick={() => setConfirmRestart(false)}
          disabled={processing}
          color="secondary"
        >
          {LL.CANCEL()}
        </Button>
        <Button
          startIcon={<PowerSettingsNewIcon />}
          variant="outlined"
          onClick={restart}
          disabled={processing}
          color="primary"
        >
          {LL.RESTART()}
        </Button>
        <Button
          startIcon={<PowerSettingsNewIcon />}
          variant="outlined"
          onClick={partition}
          disabled={processing}
          color="primary"
        >
          EMS-ESP Loader
        </Button>
      </DialogActions>
    </Dialog>
  );

  const renderFactoryResetDialog = () => (
    <Dialog sx={dialogStyle} open={confirmFactoryReset} onClose={() => setConfirmFactoryReset(false)}>
      <DialogTitle>{LL.FACTORY_RESET()}</DialogTitle>
      <DialogContent dividers>{LL.SYSTEM_FACTORY_TEXT_DIALOG()}</DialogContent>
      <DialogActions>
        <Button
          startIcon={<CancelIcon />}
          variant="outlined"
          onClick={() => setConfirmFactoryReset(false)}
          disabled={processing}
          color="secondary"
        >
          {LL.CANCEL()}
        </Button>
        <Button
          startIcon={<SettingsBackupRestoreIcon />}
          variant="outlined"
          onClick={factoryReset}
          disabled={processing}
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
        {/* TODO: translate */}
        <ListMenuItem
          icon={TuneIcon}
          bgcolor="#134ba2"
          label={LL.APPLICATION_SETTINGS()}
          text="Modify EMS-ESP Application Settings"
          to="ems-esp"
        />

        <ListMenuItem
          icon={SettingsEthernetIcon}
          bgcolor="#40828f"
          label={LL.NETWORK(0)}
          text={LL.CONFIGURE(LL.SETTINGS_OF(LL.NETWORK(0)))}
          to="network"
        />

        <ListMenuItem
          icon={AccessTimeIcon}
          bgcolor="#5f9a5f"
          label={LL.ACCESS_POINT(0)}
          text={LL.CONFIGURE(LL.ACCESS_POINT(0))}
          to="ap"
        />

        <ListMenuItem
          icon={AccessTimeIcon}
          bgcolor="#c5572c"
          label="NTP"
          text={LL.CONFIGURE(LL.LOCAL_TIME())}
          to="ntp"
        />

        <ListMenuItem icon={DeviceHubIcon} bgcolor="#68374d" label="MQTT" text={LL.CONFIGURE('MQTT')} to="mqtt" />

        <ListMenuItem icon={CastIcon} bgcolor="#efc34b" label="OTA" text={LL.CONFIGURE('OTA')} to="ota" />

        {/* TODO: translate */}
        <ListMenuItem icon={LockIcon} label={LL.SECURITY(0)} text="Add/Remove Users" to="security" />

        <ListMenuItem
          icon={MemoryIcon}
          bgcolor="#b1395f"
          label={LL.STATUS_OF('ESP32')}
          text="ESP32 Information"
          to="espsystemstatus"
        />

        {/* TODO: translate */}
        <ListMenuItem
          icon={ImportExportIcon}
          bgcolor="#5d89f7"
          label={LL.UPLOAD_DOWNLOAD()}
          text="Upload/Download Settings and Firmware"
          to="upload"
        />
      </List>

      {renderRestartDialog()}
      {renderFactoryResetDialog()}

      <Box display="flex" flexWrap="wrap">
        <Box flexGrow={1} sx={{ '& button': { mt: 2 } }}>
          <ButtonRow>
            <Button
              startIcon={<PowerSettingsNewIcon />}
              variant="outlined"
              color="primary"
              onClick={() => setConfirmRestart(true)}
            >
              {LL.RESTART()}
            </Button>
          </ButtonRow>
        </Box>
        <Box flexWrap="nowrap" whiteSpace="nowrap">
          <ButtonRow>
            <Button
              startIcon={<SettingsBackupRestoreIcon />}
              variant="outlined"
              onClick={() => setConfirmFactoryReset(true)}
              color="error"
            >
              {LL.FACTORY_RESET()}
            </Button>
          </ButtonRow>
        </Box>
      </Box>
    </>
  );

  return <SectionContent>{restarting ? <RestartMonitor /> : content()}</SectionContent>;
};

export default Settings;
