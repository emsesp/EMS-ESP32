import { FC, useState, useContext, useEffect } from 'react';
import { useSnackbar } from 'notistack';
import {
  Avatar,
  Button,
  Divider,
  Table,
  TableContainer,
  TableBody,
  TableCell,
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
  DialogTitle,
  Typography
} from '@mui/material';

import DeviceHubIcon from '@mui/icons-material/DeviceHub';
import RefreshIcon from '@mui/icons-material/Refresh';
import PermScanWifiIcon from '@mui/icons-material/PermScanWifi';
import CancelIcon from '@mui/icons-material/Cancel';

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

export const qualityHighlight = (value: number, theme: Theme) => {
  if (value >= 95) {
    return theme.palette.success.main;
  }
  return theme.palette.error.main;
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
                <DeviceHubIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary="EMS Bus Connection Status" secondary={busStatus(data) + txMode(data)} />
          </ListItem>
          <Divider variant="inset" component="li" />
          <Box mt={2} mb={2} color="warning.main">
            <Typography variant="body2">
              All statistics below are recorded from when EMS-ESP was last restarted.
            </Typography>
          </Box>
          <TableContainer>
            <Table size="small">
              <TableBody>
                <TableRow>
                  <TableCell>Recognized Devices / Sensors</TableCell>
                  <TableCell align="right">
                    {data.num_devices}&nbsp;/&nbsp;
                    {data.num_sensors}&nbsp;+&nbsp;
                    {data.num_analogs}
                  </TableCell>
                </TableRow>
                <TableRow>
                  <TableCell>Telegrams Received / Failed (Quality)</TableCell>
                  <TableCell align="right">
                    {Intl.NumberFormat().format(data.rx_received)}&nbsp;/&nbsp;
                    {Intl.NumberFormat().format(data.rx_fails)}&nbsp;({data.rx_quality}%)
                  </TableCell>
                </TableRow>
                <TableRow>
                  <TableCell>Telegrams Sent / Failed (Quality)</TableCell>
                  <TableCell align="right">
                    {Intl.NumberFormat().format(data.tx_sent)}&nbsp;/&nbsp;
                    {Intl.NumberFormat().format(data.tx_fails)}&nbsp;({data.tx_quality}%)
                  </TableCell>
                </TableRow>
                <TableRow>
                  <TableCell>Sensors Reads / Fails (Quality)</TableCell>
                  <TableCell align="right">
                    {Intl.NumberFormat().format(data.sensor_reads)}&nbsp;/&nbsp;
                    {Intl.NumberFormat().format(data.sensor_fails)}&nbsp;({data.sensor_quality}%)
                  </TableCell>
                </TableRow>
                <TableRow>
                  <TableCell>Analog Reads / Fails (Quality)</TableCell>
                  <TableCell align="right">
                    {Intl.NumberFormat().format(data.analog_reads)}&nbsp;/&nbsp;
                    {Intl.NumberFormat().format(data.analog_fails)}&nbsp;({data.analog_quality}%)
                  </TableCell>
                </TableRow>
                <TableRow>
                  <TableCell>MQTT Publishes / Fails (Quality)</TableCell>
                  <TableCell align="right">
                    {Intl.NumberFormat().format(data.mqtt_count)}&nbsp;/&nbsp;
                    {Intl.NumberFormat().format(data.mqtt_fails)}&nbsp;({data.mqtt_quality}%)
                  </TableCell>
                </TableRow>
                <TableRow>
                  <TableCell>API Calls / Fails (Quality)</TableCell>
                  <TableCell align="right">
                    {Intl.NumberFormat().format(data.api_calls)}&nbsp;/&nbsp;
                    {Intl.NumberFormat().format(data.api_fails)}&nbsp;({data.api_quality}%)
                  </TableCell>
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
