import AccessTimeIcon from '@mui/icons-material/AccessTime';
import BuildIcon from '@mui/icons-material/Build';
import CancelIcon from '@mui/icons-material/Cancel';
import CastIcon from '@mui/icons-material/Cast';
import DeviceHubIcon from '@mui/icons-material/DeviceHub';
import DirectionsBusIcon from '@mui/icons-material/DirectionsBus';
import MemoryIcon from '@mui/icons-material/Memory';
import PermScanWifiIcon from '@mui/icons-material/PermScanWifi';
import RefreshIcon from '@mui/icons-material/Refresh';
import SettingsInputAntennaIcon from '@mui/icons-material/SettingsInputAntenna';
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
import ListMenuItem from 'components/layout/ListMenuItem';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import * as EMSESP from 'project/api';
import { busConnectionStatus } from 'project/types';
import { NTPSyncStatus } from 'types';

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

  function formatNumber(num: number) {
    return new Intl.NumberFormat().format(num);
  }

  const busStatus = () => {
    if (data) {
      switch (data.status) {
        case busConnectionStatus.BUS_STATUS_CONNECTED:
          return LL.CONNECTED(0) + ' (' + formatDurationSec(data.bus_uptime) + ')';
        case busConnectionStatus.BUS_STATUS_TX_ERRORS:
          return LL.TX_ISSUES();
        case busConnectionStatus.BUS_STATUS_OFFLINE:
          return LL.DISCONNECTED();
      }
    }
    return 'Unknown';
  };

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

  const ntpStatus = () => {
    switch (data.ntp_status) {
      case NTPSyncStatus.NTP_DISABLED:
        return LL.NOT_ENABLED();
      case NTPSyncStatus.NTP_INACTIVE:
        return LL.INACTIVE(0);
      case NTPSyncStatus.NTP_ACTIVE:
        return LL.ACTIVE();
      default:
        return LL.UNKNOWN();
    }
  };

  const ntpStatusHighlight = () => {
    switch (data.ntp_status) {
      case NTPSyncStatus.NTP_DISABLED:
        return theme.palette.info.main;
      case NTPSyncStatus.NTP_INACTIVE:
        return theme.palette.error.main;
      case NTPSyncStatus.NTP_ACTIVE:
        return theme.palette.success.main;
      default:
        return theme.palette.error.main;
    }
  };

  const activeHighlight = (value: boolean) => (value ? theme.palette.success.main : theme.palette.info.main);

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
              <Avatar sx={{ bgcolor: busStatusHighlight(), color: 'white' }}>
                <DirectionsBusIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary={LL.EMS_BUS_STATUS()} secondary={busStatus()} />
          </ListItem>
          <Divider variant="inset" component="li" />

          <ListMenuItem
            disabled={!me.admin}
            icon={TimerIcon}
            bgcolor="#c5572c"
            label={LL.UPTIME()}
            text={formatDurationSec(data.uptime)}
          />
          <Divider variant="inset" component="li" />

          <ListItem>
            <ListItemAvatar>
              <Avatar sx={{ bgcolor: '#5d89f7', color: 'white' }}>
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

          <ListMenuItem
            disabled={!me.admin}
            icon={BuildIcon}
            bgcolor="#134ba2"
            label={LL.EMS_ESP_VER()}
            text={data.emsesp_version}
            to="/settings/upload"
          />
          <Divider variant="inset" component="li" />

          <ListMenuItem
            disabled={!me.admin}
            icon={MemoryIcon}
            bgcolor="#68374d"
            label={LL.SYSTEM_MEMORY()}
            text={formatNumber(data.free_heap) + ' KB'}
            to="/settings/espsystemstatus"
          />
          <Divider variant="inset" component="li" />

          <ListMenuItem
            disabled={!me.admin}
            icon={DeviceHubIcon}
            bgcolor={activeHighlight(data.mqtt_status)}
            label={LL.STATUS_OF('MQTT')}
            text={data.mqtt_status ? LL.ACTIVE() : LL.INACTIVE(0)}
            to="/settings/mqtt/status"
          />
          <Divider variant="inset" component="li" />

          <ListMenuItem
            disabled={!me.admin}
            icon={AccessTimeIcon}
            bgcolor={ntpStatusHighlight()}
            label={LL.STATUS_OF('NTP')}
            text={ntpStatus()}
            to="/settings/ntp/status"
          />
          <Divider variant="inset" component="li" />

          <ListMenuItem
            disabled={!me.admin}
            icon={CastIcon}
            bgcolor={activeHighlight(data.ota_status)}
            label={LL.STATUS_OF('OTA')}
            text={data.ota_status ? LL.ACTIVE() : LL.INACTIVE(0)}
            to="/settings/ota"
          />
          <Divider variant="inset" component="li" />

          <ListMenuItem
            disabled={!me.admin}
            icon={SettingsInputAntennaIcon}
            bgcolor={activeHighlight(data.ota_status)}
            label={LL.ACCESS_POINT(0)}
            text={data.ap_status ? LL.ACTIVE() : LL.INACTIVE(0)}
            to="/settings/ap/status"
          />
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
