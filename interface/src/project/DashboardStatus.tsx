import { FC, useState, useContext, useEffect } from 'react';
import { useSnackbar } from 'notistack';
import {
  Avatar,
  Button,
  Table,
  TableContainer,
  TableBody,
  TableCell,
  TableHead,
  TableRow,
  List,
  ListItem,
  ListItemAvatar,
  ListItemText,
  Theme,
  useTheme,
  Box,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle
} from '@mui/material';

import DeviceHubIcon from '@mui/icons-material/DeviceHub';
import RefreshIcon from '@mui/icons-material/Refresh';
import PermScanWifiIcon from '@mui/icons-material/PermScanWifi';
import CancelIcon from '@mui/icons-material/Cancel';
import DirectionsBusIcon from '@mui/icons-material/DirectionsBus';

import { AuthenticatedContext } from '../contexts/authentication';

import { ButtonRow, FormLoader, SectionContent } from '../components';

import { Status, busConnectionStatus } from './types';

import * as EMSESP from './api';

import { extractErrorMessage, useRest } from '../utils';

export const isConnected = ({ status }: Status) => status !== busConnectionStatus.BUS_STATUS_OFFLINE;

const busStatusHighlight = ({ status }: Status, theme: Theme) => {
  switch (status) {
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

const busStatus = ({ status }: Status) => {
  switch (status) {
    case busConnectionStatus.BUS_STATUS_CONNECTED:
      return 'Connected';
    case busConnectionStatus.BUS_STATUS_TX_ERRORS:
      return 'Tx issues - try a different Tx-Mode';
    case busConnectionStatus.BUS_STATUS_OFFLINE:
      return 'Disconnected';
    default:
      return 'Unknown';
  }
};

const pluralize = (count: number, noun: string) =>
  `${Intl.NumberFormat().format(count)} ${noun}${count !== 1 ? 's' : ''}`;

const formatDuration = (duration_sec: number) => {
  if (duration_sec === 0) {
    return ' ';
  }
  const roundTowardsZero = duration_sec > 0 ? Math.floor : Math.ceil;
  return (
    ', ' +
    roundTowardsZero(duration_sec / 86400) +
    'd ' +
    (roundTowardsZero(duration_sec / 3600) % 24) +
    'h ' +
    (roundTowardsZero(duration_sec / 60) % 60) +
    'm ' +
    (roundTowardsZero(duration_sec) % 60) +
    's'
  );
};

const formatRow = (name: string, success: number, fail: number, quality: number) => {
  if (success === 0 && fail === 0) {
    return (
      <TableRow>
        <TableCell sx={{ color: 'gray' }}>{name}</TableCell>
        <TableCell />
        <TableCell />
        <TableCell />
      </TableRow>
    );
  }

  return (
    <TableRow>
      <TableCell>{name}</TableCell>
      <TableCell>{Intl.NumberFormat().format(success)}</TableCell>
      <TableCell>{Intl.NumberFormat().format(fail)}</TableCell>
      {showQuality(quality)}
    </TableRow>
  );
};

const showQuality = (quality: number) => {
  if (quality === 0) {
    return <TableCell />;
  }

  if (quality === 100) {
    return <TableCell sx={{ color: '#00FF7F' }}>{quality}%</TableCell>;
  }

  if (quality >= 95) {
    return <TableCell sx={{ color: 'orange' }}>{quality}%</TableCell>;
  } else {
    return <TableCell sx={{ color: 'red' }}>{quality}%</TableCell>;
  }
};

const DashboardStatus: FC = () => {
  const { loadData, data, errorMessage } = useRest<Status>({ read: EMSESP.readStatus });

  const theme = useTheme();
  const [confirmScan, setConfirmScan] = useState<boolean>(false);
  const { enqueueSnackbar } = useSnackbar();

  const { me } = useContext(AuthenticatedContext);

  useEffect(() => {
    const timer = setInterval(() => loadData(), 30000);
    return () => {
      clearInterval(timer);
    };
    // eslint-disable-next-line
  }, []);

  const scan = async () => {
    try {
      await EMSESP.scanDevices();
      enqueueSnackbar('Scanning for devices...', { variant: 'info' });
    } catch (error: any) {
      enqueueSnackbar(extractErrorMessage(error, 'Problem initiating scan'), { variant: 'error' });
    } finally {
      setConfirmScan(false);
    }
  };

  const renderScanDialog = () => (
    <Dialog open={confirmScan} onClose={() => setConfirmScan(false)}>
      <DialogTitle>EMS Device Scan</DialogTitle>
      <DialogContent dividers>Are you sure you want to initiate a full device scan of the EMS bus?</DialogContent>
      <DialogActions>
        <Button startIcon={<CancelIcon />} variant="outlined" onClick={() => setConfirmScan(false)} color="secondary">
          Cancel
        </Button>
        <Button startIcon={<PermScanWifiIcon />} variant="outlined" onClick={scan} color="primary" autoFocus>
          Scan
        </Button>
      </DialogActions>
    </Dialog>
  );

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage} />;
    }

    return (
      <>
        <List>
          <ListItem>
            <ListItemAvatar>
              <Avatar sx={{ bgcolor: busStatusHighlight(data, theme) }}>
                <DirectionsBusIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary="EMS Bus Status" secondary={busStatus(data) + formatDuration(data.uptime)} />
          </ListItem>
          <ListItem>
            <ListItemAvatar>
              <Avatar>
                <DeviceHubIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText
              primary="Active Devices &amp; Sensors"
              secondary={
                pluralize(data.num_devices, 'EMS Device') +
                ', ' +
                pluralize(data.num_sensors, 'Temperature Sensor') +
                ', ' +
                pluralize(data.num_analogs, 'Analog Sensor')
              }
            />
          </ListItem>
          <Box m={3}></Box>
          <TableContainer>
            <Table size="small">
              <TableHead>
                <TableRow>
                  <TableCell></TableCell>
                  <TableCell>SUCCESS</TableCell>
                  <TableCell>FAIL</TableCell>
                  <TableCell>QUALITY</TableCell>
                </TableRow>
              </TableHead>
              <TableBody>
                {formatRow('EMS Telegrams Received (Rx)', data.rx_received, data.rx_fails, data.rx_quality)}
                {formatRow('EMS Reads(Tx)', data.tx_reads, data.tx_read_fails, data.tx_read_quality)}
                {formatRow('EMS Writes (Tx)', data.tx_writes, data.tx_write_fails, data.tx_write_quality)}
                {formatRow('Temperature Sensor Reads', data.sensor_reads, data.sensor_fails, data.sensor_quality)}
                {formatRow('Analog Sensor Reads', data.analog_reads, data.analog_fails, data.analog_quality)}
                {formatRow('MQTT Publishes', data.mqtt_count, data.mqtt_fails, data.mqtt_quality)}
                {formatRow('API Calls', data.api_calls, data.api_fails, data.api_quality)}
              </TableBody>
            </Table>
          </TableContainer>
        </List>
        {renderScanDialog()}
        <Box display="flex" flexWrap="wrap">
          <Box flexGrow={1} sx={{ '& button': { mt: 2 } }}>
            <Button startIcon={<RefreshIcon />} variant="outlined" color="secondary" onClick={loadData}>
              Refresh
            </Button>
          </Box>
          <Box flexWrap="nowrap" whiteSpace="nowrap">
            <ButtonRow>
              <Button
                startIcon={<PermScanWifiIcon />}
                variant="outlined"
                color="primary"
                disabled={!me.admin}
                onClick={() => setConfirmScan(true)}
              >
                Scan for new devices
              </Button>
            </ButtonRow>
          </Box>
        </Box>
      </>
    );
  };

  return (
    <SectionContent title="EMS Bus &amp; Activity Status" titleGutter>
      {content()}
    </SectionContent>
  );
};

export default DashboardStatus;
