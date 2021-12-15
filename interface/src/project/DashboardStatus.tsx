import { FC, useState } from 'react';
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
  DialogTitle
} from '@mui/material';

import DeviceHubIcon from '@mui/icons-material/DeviceHub';
import RefreshIcon from '@mui/icons-material/Refresh';
import PermScanWifiIcon from '@mui/icons-material/PermScanWifi';

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
        <Button variant="outlined" onClick={() => setConfirmScan(false)} color="secondary">
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
            <ListItemText primary="EMS Bus Connection Status" secondary={busStatus(data)} />
          </ListItem>
          <Divider variant="inset" component="li" />
          {data.status !== busConnectionStatus.BUS_STATUS_OFFLINE && (
            <TableContainer>
              <Table size="small">
                <TableBody>
                  <TableRow>
                    <TableCell># Recognized devices</TableCell>
                    <TableCell align="right">{data.num_devices}</TableCell>
                  </TableRow>
                  <TableRow>
                    <TableCell>Telegrams Received</TableCell>
                    <TableCell align="right">
                      {Intl.NumberFormat().format(data.rx_received)}&nbsp;(quality {data.rx_quality}%)
                    </TableCell>
                  </TableRow>
                  <TableRow>
                    <TableCell>Telegrams Sent</TableCell>
                    <TableCell align="right">
                      {Intl.NumberFormat().format(data.tx_sent)}&nbsp;(quality {data.tx_quality}%)
                    </TableCell>
                  </TableRow>
                </TableBody>
              </Table>
            </TableContainer>
          )}
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
                onClick={() => setConfirmScan(true)}
              >
                Detect new devices
              </Button>
            </ButtonRow>
          </Box>
        </Box>
      </>
    );
  };

  return (
    <SectionContent title="EMS-ESP Status" titleGutter>
      {content()}
    </SectionContent>
  );
};

export default DashboardStatus;
