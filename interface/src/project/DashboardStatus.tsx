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

export const busStatusHighlight = ({ status }: Status, theme: Theme) => {
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

export const busStatus = ({ status }: Status) => {
  switch (status) {
    case busConnectionStatus.BUS_STATUS_CONNECTED:
      return 'Connected';
    case busConnectionStatus.BUS_STATUS_TX_ERRORS:
      return 'Tx Errors';
    case busConnectionStatus.BUS_STATUS_OFFLINE:
      return 'Disconnected';
    default:
      return 'Unknown';
  }
};

export const txMode = ({ tx_mode }: Status) => {
  switch (tx_mode) {
    case 1:
      return ', Tx-Mode: EMS';
    case 2:
      return ', Tx-Mode: EMS+';
    case 3:
      return ', Tx-Mode: HT3';
    case 4:
      return ', Tx-Mode: Hardware';
    default:
      return ', Tx-Mode: Off';
  }
};

const pluralize = (count: number, noun: string) =>
  `${Intl.NumberFormat().format(count)} ${noun}${count !== 1 ? 's' : ''}`;

const formatDuration = (duration_sec: number) => {
  const roundTowardsZero = duration_sec > 0 ? Math.floor : Math.ceil;
  return (
    roundTowardsZero(duration_sec / 8640) +
    'd ' +
    (roundTowardsZero(duration_sec / 360) % 24) +
    'h ' +
    (roundTowardsZero(duration_sec / 60) % 60) +
    'm ' +
    (roundTowardsZero(duration_sec) % 60) +
    's'
  );
};

const showQuality = (quality: number) => {
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
            <ListItemText
              primary="EMS Bus Connection Status"
              secondary={busStatus(data) + ', ' + formatDuration(data.uptime)}
            />
          </ListItem>
          <ListItem>
            <ListItemAvatar>
              <Avatar sx={{ bgcolor: busStatusHighlight(data, theme) }}>
                <DeviceHubIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText
              primary="Active Devices &amp; Sensors"
              secondary={
                pluralize(data.num_devices, 'device') +
                ', ' +
                pluralize(data.num_sensors, 'temperature sensor') +
                ', ' +
                pluralize(data.num_analogs, 'analog sensor')
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
                <TableRow>
                  <TableCell>Telegrams Received (Rx)</TableCell>
                  <TableCell>{Intl.NumberFormat().format(data.rx_received)}</TableCell>
                  <TableCell>{Intl.NumberFormat().format(data.rx_fails)}</TableCell>
                  {showQuality(data.rx_quality)}
                </TableRow>
                <TableRow>
                  <TableCell>Telegrams Sent (Tx)</TableCell>
                  <TableCell>{Intl.NumberFormat().format(data.tx_sent)}</TableCell>
                  <TableCell>{Intl.NumberFormat().format(data.tx_fails)}</TableCell>
                  {showQuality(data.tx_quality)}
                </TableRow>
                <TableRow>
                  <TableCell>Analog Sensor Reads</TableCell>
                  <TableCell>{Intl.NumberFormat().format(data.analog_reads)}</TableCell>
                  <TableCell>{Intl.NumberFormat().format(data.analog_fails)}</TableCell>
                  {showQuality(data.analog_quality)}
                </TableRow>
                <TableRow>
                  <TableCell>MQTT Publishes</TableCell>
                  <TableCell>{Intl.NumberFormat().format(data.mqtt_count)}</TableCell>
                  <TableCell>{Intl.NumberFormat().format(data.mqtt_fails)}</TableCell>
                  {showQuality(data.mqtt_quality)}
                </TableRow>
                <TableRow>
                  <TableCell>API Calls</TableCell>
                  <TableCell>{Intl.NumberFormat().format(data.api_calls)}</TableCell>
                  <TableCell>{Intl.NumberFormat().format(data.api_fails)}</TableCell>
                  {showQuality(data.api_quality)}
                </TableRow>
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
