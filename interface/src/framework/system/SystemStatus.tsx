import BuildIcon from '@mui/icons-material/Build';
import CancelIcon from '@mui/icons-material/Cancel';
import DeviceHubIcon from '@mui/icons-material/DeviceHub';
import DirectionsBusIcon from '@mui/icons-material/DirectionsBus';
import PermScanWifiIcon from '@mui/icons-material/PermScanWifi';
import RefreshIcon from '@mui/icons-material/Refresh';
import TimerIcon from '@mui/icons-material/Timer';

import {
  Avatar,
  Box,
  Button,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  Divider,
  List,
  ListItem,
  ListItemAvatar,
  ListItemText,
  useTheme
} from '@mui/material';

import { useRequest } from 'alova';
import { useContext, type FC, useState } from 'react';

import { toast } from 'react-toastify';
import { dialogStyle } from 'CustomTheme';
import * as SystemApi from 'api/system';
import { FormLoader, SectionContent, useLayoutTitle } from 'components';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import * as EMSESP from 'project/api';
import { busConnectionStatus } from 'project/types';

const SystemStatus: FC = () => {
  const { LL } = useI18nContext();

  useLayoutTitle(LL.STATUS_OF(''));

  const { me } = useContext(AuthenticatedContext);

  const [confirmScan, setConfirmScan] = useState<boolean>(false);

  const { data: data, send: loadData, error } = useRequest(SystemApi.readSystemStatus, { force: true });

  const { send: scanDevices } = useRequest(EMSESP.scanDevices, {
    immediate: false
  });

  const theme = useTheme();

  const formatDurationSec = (duration_sec: number) => {
    const days = Math.trunc((duration_sec * 1000) / 86400000);
    const hours = Math.trunc((duration_sec * 1000) / 3600000) % 24;
    const minutes = Math.trunc((duration_sec * 1000) / 60000) % 60;
    const seconds = Math.trunc((duration_sec * 1000) / 1000) % 60;

    let formatted = '';
    if (days) {
      formatted += LL.NUM_DAYS({ num: days }) + ' ';
    }
    if (hours) {
      formatted += LL.NUM_HOURS({ num: hours }) + ' ';
    }
    if (minutes) {
      formatted += LL.NUM_MINUTES({ num: minutes }) + ' ';
    }
    formatted += LL.NUM_SECONDS({ num: seconds });
    return formatted;
  };

  const busStatus = () => {
    if (data) {
      switch (data.status) {
        case busConnectionStatus.BUS_STATUS_CONNECTED:
          return LL.CONNECTED(0);
        case busConnectionStatus.BUS_STATUS_TX_ERRORS:
          return LL.TX_ISSUES();
        case busConnectionStatus.BUS_STATUS_OFFLINE:
          return LL.DISCONNECTED();
      }
    }
    return 'Unknown';
  };

  // export const isConnected = ({ status }: Status) => status !== busConnectionStatus.BUS_STATUS_OFFLINE;

  const busStatusHighlight = () => {
    switch (data.status) {
      case busConnectionStatus.BUS_STATUS_TX_ERRORS:
        return theme.palette.warning.main;
      case busConnectionStatus.BUS_STATUS_CONNECTED:
        return theme.palette.success.main;
      case busConnectionStatus.BUS_STATUS_OFFLINE:
        return theme.palette.error.main;
      default:
        return theme.palette.warning.main;
    }
  };

  const scan = async () => {
    await scanDevices()
      .then(() => {
        toast.info(LL.SCANNING() + '...');
      })
      .catch((err) => {
        toast.error(err.message);
      });
    setConfirmScan(false);
  };

  const renderScanDialog = () => (
    <Dialog sx={dialogStyle} open={confirmScan} onClose={() => setConfirmScan(false)}>
      <DialogTitle>{LL.SCAN_DEVICES()}</DialogTitle>
      <DialogContent dividers>{LL.EMS_SCAN()}</DialogContent>
      <DialogActions>
        <Button startIcon={<CancelIcon />} variant="outlined" onClick={() => setConfirmScan(false)} color="secondary">
          {LL.CANCEL()}
        </Button>
        <Button startIcon={<PermScanWifiIcon />} variant="outlined" onClick={scan} color="primary">
          {LL.SCAN()}
        </Button>
      </DialogActions>
    </Dialog>
  );

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={error?.message} />;
    }

    return (
      <>
        <List>
          <ListItem>
            <ListItemAvatar>
              <Avatar>
                <BuildIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary={LL.EMS_ESP_VER()} secondary={data.emsesp_version} />
          </ListItem>
          <Divider variant="inset" component="li" />

          <ListItem>
            <ListItemAvatar>
              <Avatar sx={{ bgcolor: '#5f9a5f', color: 'white' }}>
                <TimerIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary={LL.UPTIME()} secondary={formatDurationSec(data.uptime)} />
          </ListItem>
          <Divider variant="inset" component="li" />

          <ListItem>
            <ListItemAvatar>
              <Avatar sx={{ bgcolor: busStatusHighlight() }}>
                <DirectionsBusIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary={LL.EMS_BUS_STATUS()} secondary={busStatus()} />
          </ListItem>
          <Divider variant="inset" component="li" />

          <ListItem>
            <ListItemAvatar>
              <Avatar sx={{ bgcolor: theme.palette.success.main }}>
                <DeviceHubIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText
              primary={LL.ACTIVE_DEVICES()}
              secondary={
                LL.NUM_DEVICES({ num: data.num_devices }) +
                ', ' +
                LL.NUM_TEMP_SENSORS({ num: data.num_sensors }) +
                ', ' +
                LL.NUM_ANALOG_SENSORS({ num: data.num_analogs })
              }
            />
            {me.admin && (
              <Button
                startIcon={<PermScanWifiIcon />}
                variant="outlined"
                color="primary"
                onClick={() => setConfirmScan(true)}
              >
                {LL.SCAN_DEVICES()}
              </Button>
            )}
          </ListItem>

          <Divider variant="inset" component="li" />
        </List>

        {renderScanDialog()}

        <Box mt={2} display="flex" flexWrap="wrap">
          <Button startIcon={<RefreshIcon />} variant="outlined" color="secondary" onClick={loadData}>
            {LL.REFRESH()}
          </Button>
        </Box>
      </>
    );
  };

  return <SectionContent>{content()}</SectionContent>;
};

export default SystemStatus;
