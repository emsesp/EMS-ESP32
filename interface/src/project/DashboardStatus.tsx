import { FC, useState, useContext, useEffect } from 'react';
import { useSnackbar } from 'notistack';
import {
  Avatar,
  Button,
  List,
  ListItem,
  ListItemAvatar,
  ListItemText,
  Box,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  Theme,
  useTheme
} from '@mui/material';

import { Table } from '@table-library/react-table-library/table';
import { useTheme as tableTheme } from '@table-library/react-table-library/theme';
import { Header, HeaderRow, HeaderCell, Body, Row, Cell } from '@table-library/react-table-library/table';

import DeviceHubIcon from '@mui/icons-material/DeviceHub';
import RefreshIcon from '@mui/icons-material/Refresh';
import PermScanWifiIcon from '@mui/icons-material/PermScanWifi';
import CancelIcon from '@mui/icons-material/Cancel';
import DirectionsBusIcon from '@mui/icons-material/DirectionsBus';

import { AuthenticatedContext } from '../contexts/authentication';

import { ButtonRow, FormLoader, SectionContent } from '../components';

import { Status, busConnectionStatus, Stat } from './types';

import { formatDurationSec, pluralize, extractErrorMessage, useRest } from '../utils';

import * as EMSESP from './api';

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
      return 'Tx issues - try a different Tx Mode';
    case busConnectionStatus.BUS_STATUS_OFFLINE:
      return 'Disconnected';
    default:
      return 'Unknown';
  }
};

const showQuality = (stat: Stat) => {
  if (stat.q === 0 || stat.s + stat.f === 0) {
    return;
  }
  if (stat.q === 100) {
    return <div style={{ color: '#00FF7F' }}>{stat.q}%</div>;
  }
  if (stat.q >= 95) {
    return <div style={{ color: 'orange' }}>{stat.q}%</div>;
  } else {
    return <div style={{ color: 'red' }}>{stat.q}%</div>;
  }
};

const DashboardStatus: FC = () => {
  const { loadData, data, errorMessage } = useRest<Status>({ read: EMSESP.readStatus });

  const theme = useTheme();
  const [confirmScan, setConfirmScan] = useState<boolean>(false);
  const { enqueueSnackbar } = useSnackbar();

  const { me } = useContext(AuthenticatedContext);

  const stats_theme = tableTheme({
    Table: `
      --data-table-library_grid-template-columns: repeat(1, minmax(0, 1fr)) 90px 90px 80px;
    `,
    BaseRow: `
      font-size: 14px;
    `,
    HeaderRow: `
      text-transform: uppercase;
      background-color: black;
      color: #90CAF9;

      .th {
        height: 42px;
        font-weight: 500;
        border-bottom: 1px solid #565656;
      }
    `,
    Row: `
      .td {
        padding: 8px;
        border-top: 1px solid #565656;
        border-bottom: 1px solid #565656;
      }

      &:nth-of-type(odd) .td {
        background-color: #303030;
      }
      &:nth-of-type(even) .td {
        background-color: #1e1e1e;
      }
    `,
    BaseCell: `
      &:not(:first-of-type) {
        text-align: center;
      }
    `
  });

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
    } catch (error: unknown) {
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
            <ListItemText primary="EMS Bus Status" secondary={busStatus(data) + formatDurationSec(data.uptime)} />
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
          <Table data={{ nodes: data.stats }} theme={stats_theme} layout={{ custom: true }}>
            {(tableList: any) => (
              <>
                <Header>
                  <HeaderRow>
                    <HeaderCell resize></HeaderCell>
                    <HeaderCell stiff>SUCCESS</HeaderCell>
                    <HeaderCell stiff>FAIL</HeaderCell>
                    <HeaderCell stiff>QUALITY</HeaderCell>
                  </HeaderRow>
                </Header>
                <Body>
                  {tableList.map((stat: Stat) => (
                    <Row key={stat.id} item={stat}>
                      <Cell>{stat.id}</Cell>
                      <Cell stiff>{Intl.NumberFormat().format(stat.s)}</Cell>
                      <Cell stiff>{Intl.NumberFormat().format(stat.f)}</Cell>
                      <Cell stiff>{showQuality(stat)}</Cell>
                    </Row>
                  ))}
                </Body>
              </>
            )}
          </Table>
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
