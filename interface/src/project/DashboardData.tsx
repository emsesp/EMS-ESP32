import { FC, useState, useContext, useCallback, useEffect } from 'react';

import {
  Button,
  Typography,
  Box,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  MenuItem,
  InputAdornment,
  FormHelperText,
  IconButton,
  List,
  ListItem,
  ListItemText,
  Grid,
  FormControlLabel,
  Checkbox
} from '@mui/material';

import { useSnackbar } from 'notistack';

import { Table } from '@table-library/react-table-library/table';
import { useTheme } from '@table-library/react-table-library/theme';
import { useSort } from '@table-library/react-table-library/sort';
import { Header, HeaderRow, HeaderCell, Body, Row, Cell } from '@table-library/react-table-library/table';
import { useRowSelect } from '@table-library/react-table-library/select';

import DownloadIcon from '@mui/icons-material/GetApp';
import RefreshIcon from '@mui/icons-material/Refresh';
import EditIcon from '@mui/icons-material/Edit';
import InfoOutlinedIcon from '@mui/icons-material/InfoOutlined';
import AddCircleOutlineOutlinedIcon from '@mui/icons-material/AddCircleOutlineOutlined';
import CancelIcon from '@mui/icons-material/Cancel';
import SendIcon from '@mui/icons-material/TrendingFlat';
import SaveIcon from '@mui/icons-material/Save';
import RemoveIcon from '@mui/icons-material/RemoveCircleOutline';
import PlayArrowIcon from '@mui/icons-material/PlayArrow';
import EditOffOutlinedIcon from '@mui/icons-material/EditOffOutlined';
import CommentsDisabledOutlinedIcon from '@mui/icons-material/CommentsDisabledOutlined';
import KeyboardArrowUpOutlinedIcon from '@mui/icons-material/KeyboardArrowUpOutlined';
import KeyboardArrowDownOutlinedIcon from '@mui/icons-material/KeyboardArrowDownOutlined';
import StarIcon from '@mui/icons-material/Star';

import DeviceIcon from './DeviceIcon';

import { IconContext } from 'react-icons';

import { formatDurationMin, pluralize } from '../utils';

import { AuthenticatedContext } from '../contexts/authentication';

import { ButtonRow, ValidatedTextField, SectionContent, MessageBox } from '../components';

import * as EMSESP from './api';

import { numberValue, updateValue, extractErrorMessage } from '../utils';

import {
  SensorData,
  Device,
  CoreData,
  DeviceData,
  DeviceValue,
  DeviceValueUOM,
  DeviceValueUOM_s,
  AnalogType,
  AnalogTypeNames,
  Sensor,
  Analog,
  DeviceEntityMask
} from './types';

const DashboardData: FC = () => {
  const { me } = useContext(AuthenticatedContext);

  const { enqueueSnackbar } = useSnackbar();

  const [coreData, setCoreData] = useState<CoreData>({ devices: [], active_sensors: 0, analog_enabled: false });
  const [deviceData, setDeviceData] = useState<DeviceData>({ label: '', data: [] });
  const [sensorData, setSensorData] = useState<SensorData>({ sensors: [], analogs: [] });
  const [deviceValue, setDeviceValue] = useState<DeviceValue>();
  const [sensor, setSensor] = useState<Sensor>();
  const [analog, setAnalog] = useState<Analog>();
  const [deviceDialog, setDeviceDialog] = useState<number>(-1);
  const [onlyFav, setOnlyFav] = useState(false);

  const device_theme = useTheme({
    BaseRow: `
      font-size: 14px;
      color: white;
      height: 46px;
      &:focus {
        z-index: 2;
        border-top: 1px solid #177ac9;
        border-bottom: 1px solid #177ac9;
      }
    `,
    HeaderRow: `
      text-transform: uppercase;
      background-color: black;
      border-bottom: 1px solid #e0e0e0;
    `,
    Row: `
      background-color: #1e1e1e;
      border-top: 1px solid #565656;
      border-bottom: 1px solid #565656;
      position: relative;
      z-index: 1;
      &:not(:last-of-type) {
        margin-bottom: -1px;
      }
      &:not(:first-of-type) {
        margin-top: -1px;
      }
      &:hover {
        z-index: 2;
        color: white;
        border-top: 1px solid #177ac9;
        border-bottom: 1px solid #177ac9;
      },
      &.tr.tr-body.row-select.row-select-single-selected, &.tr.tr-body.row-select.row-select-selected {
        background-color: #3d4752;
        color: white;
        font-weight: normal;
        z-index: 2;
        border-top: 1px solid #177ac9;
        border-bottom: 1px solid #177ac9;
      }
    `,
    BaseCell: `
      border-top: 1px solid transparent;
      border-right: 1px solid transparent;
      border-bottom: 1px solid transparent;
      &:nth-of-type(1) {
        min-width: 42px;
        width: 42px;
        div {
          width: 100%;
        }
      }
      &:nth-of-type(2) {
        min-width: 120px;
        width: 120px;
      }
      &:nth-of-type(3) {
        flex: 1;
      }
      &:nth-of-type(4) {
        text-align: center;
        max-width: 100px;
      }
      &:last-of-type {
        text-align: right;
        min-width: 64px;
      }
    `
  });

  const data_theme = useTheme({
    BaseRow: `
      font-size: 14px;
      color: white;
      height: 32px;
    `,
    HeaderRow: `
      text-transform: uppercase;
      background-color: black;
      color: #90CAF9;
      border-bottom: 1px solid #e0e0e0;
    `,
    Row: `
      &:nth-of-type(odd) {
        background-color: #303030;
      }
      &:nth-of-type(even) {
        background-color: #1e1e1e;
      }
      border-top: 1px solid #565656;
      border-bottom: 1px solid #565656;
      position: relative;
      z-index: 1;
      &:not(:last-of-type) {
        margin-bottom: -1px;
      }
      &:not(:first-of-type) {
        margin-top: -1px;
      }
      &:hover {
        z-index: 2;
        border-top: 1px solid #177ac9;
        border-bottom: 1px solid #177ac9;
        color: white;
        cursor: 'pointer',
      }
    `,
    BaseCell: `
      padding-left: 8px;
      border-top: 1px solid transparent;
      border-right: 1px solid transparent;
      border-bottom: 1px solid transparent;
      &:last-of-type {
        text-align: right;
        min-width: 64px;
      }
    `,
    HeaderCell: `
      padding-left: 0px;
    `
  });

  const getSortIcon = (state: any, sortKey: any) => {
    if (state.sortKey === sortKey && state.reverse) {
      return <KeyboardArrowDownOutlinedIcon />;
    }

    if (state.sortKey === sortKey && !state.reverse) {
      return <KeyboardArrowUpOutlinedIcon />;
    }
  };

  const analog_sort = useSort(
    { nodes: sensorData.analogs },
    {
      state: {
        sortKey: 'GPIO',
        reverse: false
      }
    },
    {
      sortIcon: {
        iconDefault: null,
        iconUp: <KeyboardArrowUpOutlinedIcon />,
        iconDown: <KeyboardArrowDownOutlinedIcon />
      },
      sortFns: {
        GPIO: (array) => array.sort((a, b) => a.g - b.g),
        NAME: (array) => array.sort((a, b) => a.n.localeCompare(b.n)),
        TYPE: (array) => array.sort((a, b) => a.t - b.t),
        VALUE: (array) => array.sort((a, b) => a.v.toString().localeCompare(b.v.toString()))
      }
    }
  );

  const sensor_sort = useSort(
    { nodes: sensorData.sensors },
    {
      state: {
        sortKey: 'NAME',
        reverse: false
      }
    },
    {
      sortIcon: {
        iconDefault: null,
        iconUp: <KeyboardArrowUpOutlinedIcon />,
        iconDown: <KeyboardArrowDownOutlinedIcon />
      },
      sortFns: {
        NAME: (array) => array.sort((a, b) => a.id.localeCompare(b.id)),
        TEMPERATURE: (array) => array.sort((a, b) => a.id.localeCompare(b.id))
      }
    }
  );

  const dv_sort = useSort(
    { nodes: deviceData.data },
    {
      state: {
        sortKey: 'NAME',
        reverse: false
      }
    },
    {
      sortIcon: {
        iconDefault: null,
        iconUp: <KeyboardArrowUpOutlinedIcon />,
        iconDown: <KeyboardArrowDownOutlinedIcon />
      },
      sortFns: {
        NAME: (array) => array.sort((a, b) => a.id.slice(2).localeCompare(b.id.slice(2))),
        VALUE: (array) => array.sort((a, b) => a.v.toString().localeCompare(b.v.toString()))
      }
    }
  );

  const device_select = useRowSelect(
    { nodes: coreData.devices },
    {
      onChange: onSelectChange
    }
  );

  function onSelectChange(action: any, state: any) {
    if (action.type === 'ADD_BY_ID_EXCLUSIVELY') {
      refreshData();
    } else {
      setSensorData({ sensors: [], analogs: [] });
    }
  }

  const escapeCsvCell = (cell: any) => {
    if (cell == null) {
      return '';
    }
    const sc = cell.toString().trim();
    if (sc === '' || sc === '""') {
      return sc;
    }
    if (sc.includes('"') || sc.includes(',') || sc.includes('\n') || sc.includes('\r')) {
      return '"' + sc.replace(/"/g, '""') + '"';
    }
    return sc;
  };

  const makeCsvData = (columns: any, data: any) => {
    return data.reduce((csvString: any, rowItem: any) => {
      return csvString + columns.map(({ accessor }: any) => escapeCsvCell(accessor(rowItem))).join(',') + '\r\n';
    }, columns.map(({ name }: any) => escapeCsvCell(name)).join(',') + '\r\n');
  };

  const downloadAsCsv = (columns: any, data: any, filename: string) => {
    const csvData = makeCsvData(columns, data);
    const csvFile = new Blob([csvData], { type: 'text/csv' });
    const downloadLink = document.createElement('a');

    downloadLink.download = filename;
    downloadLink.href = window.URL.createObjectURL(csvFile);
    document.body.appendChild(downloadLink);
    downloadLink.click();
    document.body.removeChild(downloadLink);
  };

  const hasMask = (id: string, mask: number) => (parseInt(id.slice(0, 2), 16) & mask) === mask;

  const handleDownloadCsv = () => {
    const columns = [
      { accessor: (dv: any) => dv.id.slice(2), name: 'Entity' },
      { accessor: (dv: any) => dv.v, name: 'Value' },
      { accessor: (dv: any) => (dv.u >= 1 && dv.u <= 2 ? 'C' : DeviceValueUOM_s[dv.u]), name: 'UoM' }
    ];
    downloadAsCsv(
      columns,
      onlyFav ? deviceData.data.filter((dv) => hasMask(dv.id, DeviceEntityMask.DV_FAVORITE)) : deviceData.data,
      'device_entities'
    );
  };

  const refreshData = () => {
    const selectedDevice = device_select.state.id;
    if (selectedDevice === 'sensor') {
      fetchSensorData();
      return;
    }

    setSensorData({ sensors: [], analogs: [] });
    if (selectedDevice) {
      fetchDeviceData(selectedDevice);
    } else {
      fetchCoreData();
    }
  };

  const fetchCoreData = useCallback(async () => {
    try {
      setCoreData((await EMSESP.readCoreData()).data);
    } catch (error: any) {
      enqueueSnackbar(extractErrorMessage(error, 'Failed to fetch core data'), { variant: 'error' });
    }
  }, [enqueueSnackbar]);

  useEffect(() => {
    fetchCoreData();
  }, [fetchCoreData]);

  useEffect(() => {
    const timer = setInterval(() => refreshData(), 60000);
    return () => {
      clearInterval(timer);
    };
    // eslint-disable-next-line
  }, [analog, sensor, deviceValue, sensorData]);

  const fetchDeviceData = async (id: string) => {
    const unique_id = parseInt(id);
    try {
      setDeviceData((await EMSESP.readDeviceData({ id: unique_id })).data);
    } catch (error: any) {
      enqueueSnackbar(extractErrorMessage(error, 'Problem fetching device data'), { variant: 'error' });
    }
  };

  const fetchSensorData = async () => {
    try {
      setSensorData((await EMSESP.readSensorData()).data);
    } catch (error: any) {
      enqueueSnackbar(extractErrorMessage(error, 'Problem fetching sensor data'), { variant: 'error' });
    }
  };

  const isCmdOnly = (dv: DeviceValue) => dv.v === undefined && dv.c;

  function formatValue(value: any, uom: number) {
    if (value === undefined) {
      return '';
    }
    switch (uom) {
      case DeviceValueUOM.HOURS:
        return value ? formatDurationMin(value * 60) : '0 hours';
      case DeviceValueUOM.MINUTES:
        return value ? formatDurationMin(value) : '0 minutes';
      case DeviceValueUOM.NONE:
        if (typeof value === 'number') {
          return new Intl.NumberFormat().format(value);
        }
        return value;
      case DeviceValueUOM.DEGREES:
      case DeviceValueUOM.DEGREES_R:
      case DeviceValueUOM.FAHRENHEIT:
        return (
          new Intl.NumberFormat(undefined, {
            minimumFractionDigits: 1
          }).format(value) +
          ' ' +
          DeviceValueUOM_s[uom]
        );
      case DeviceValueUOM.SECONDS:
        return pluralize(value, DeviceValueUOM_s[uom]);
      default:
        return new Intl.NumberFormat().format(value) + ' ' + DeviceValueUOM_s[uom];
    }
  }

  const sendDeviceValue = async () => {
    if (deviceValue) {
      try {
        const response = await EMSESP.writeValue({
          id: Number(device_select.state.id),
          devicevalue: deviceValue
        });
        if (response.status === 204) {
          enqueueSnackbar('Write command failed', { variant: 'error' });
        } else if (response.status === 403) {
          enqueueSnackbar('Write access denied', { variant: 'error' });
        } else {
          enqueueSnackbar('Write command sent', { variant: 'success' });
        }
        setDeviceValue(undefined);
      } catch (error: any) {
        enqueueSnackbar(extractErrorMessage(error, 'Problem writing value'), { variant: 'error' });
      } finally {
        refreshData();
        setDeviceValue(undefined);
      }
    }
  };

  const renderDeviceValueDialog = () => {
    if (deviceValue) {
      return (
        <Dialog open={deviceValue !== undefined} onClose={() => setDeviceValue(undefined)}>
          <DialogTitle>{isCmdOnly(deviceValue) ? 'Run Command' : 'Change Value'}</DialogTitle>
          <DialogContent dividers>
            {deviceValue.l && (
              <ValidatedTextField
                name="v"
                label={deviceValue.id.slice(2)}
                value={deviceValue.v}
                autoFocus
                sx={{ width: '30ch' }}
                select
                onChange={updateValue(setDeviceValue)}
              >
                {deviceValue.l.map((val) => (
                  <MenuItem value={val}>{val}</MenuItem>
                ))}
              </ValidatedTextField>
            )}
            {!deviceValue.l && (
              <ValidatedTextField
                name="v"
                label={deviceValue.id.slice(2)}
                value={deviceValue.u ? numberValue(deviceValue.v) : deviceValue.v}
                autoFocus
                sx={{ width: '30ch' }}
                type={deviceValue.u ? 'number' : 'text'}
                onChange={updateValue(setDeviceValue)}
                inputProps={deviceValue.u ? { min: deviceValue.m, max: deviceValue.x, step: deviceValue.s } : {}}
                InputProps={{
                  startAdornment: <InputAdornment position="start">{DeviceValueUOM_s[deviceValue.u]}</InputAdornment>
                }}
              />
            )}
            {deviceValue.h && <FormHelperText>{deviceValue.h}</FormHelperText>}
          </DialogContent>
          <DialogActions>
            <Button
              startIcon={<CancelIcon />}
              variant="outlined"
              onClick={() => setDeviceValue(undefined)}
              color="secondary"
            >
              Cancel
            </Button>
            <Button
              startIcon={<SendIcon />}
              variant="outlined"
              type="submit"
              onClick={() => sendDeviceValue()}
              color="warning"
            >
              Send
            </Button>
          </DialogActions>
        </Dialog>
      );
    }
  };

  const addAnalogSensor = () => {
    setAnalog({ id: '0', g: 0, n: '', u: 0, v: 0, o: 0, t: 0, f: 1 });
  };

  const sendSensor = async () => {
    if (sensor) {
      try {
        const response = await EMSESP.writeSensor({
          id: sensor.id,
          name: sensor.n,
          offset: sensor.o
        });
        if (response.status === 204) {
          enqueueSnackbar('Sensor change failed', { variant: 'error' });
        } else if (response.status === 403) {
          enqueueSnackbar('Access denied', { variant: 'error' });
        } else {
          enqueueSnackbar('Sensor updated', { variant: 'success' });
        }
        setSensor(undefined);
      } catch (error: any) {
        enqueueSnackbar(extractErrorMessage(error, 'Problem updating sensor'), { variant: 'error' });
      } finally {
        setSensor(undefined);
        fetchSensorData();
      }
    }
  };

  const renderSensorDialog = () => {
    if (sensor) {
      return (
        <Dialog open={sensor !== undefined} onClose={() => setSensor(undefined)}>
          <DialogTitle>Edit Temperature Sensor</DialogTitle>
          <DialogContent dividers>
            <Box color="warning.main" p={0} pl={0} pr={0} mt={0} mb={2}>
              <Typography variant="body2">Sensor ID {sensor.id}</Typography>
            </Box>
            <Grid container spacing={1}>
              <Grid item>
                <ValidatedTextField
                  name="n"
                  label="Name"
                  value={sensor.n}
                  autoFocus
                  sx={{ width: '30ch' }}
                  onChange={updateValue(setSensor)}
                />
              </Grid>
              <Grid item>
                <ValidatedTextField
                  name="o"
                  label="Offset"
                  value={numberValue(sensor.o)}
                  sx={{ width: '12ch' }}
                  type="number"
                  variant="outlined"
                  onChange={updateValue(setSensor)}
                  inputProps={{ min: '-5', max: '5', step: '0.1' }}
                  InputProps={{
                    startAdornment: <InputAdornment position="start">°C</InputAdornment>
                  }}
                />
              </Grid>
            </Grid>
          </DialogContent>
          <DialogActions>
            <Button
              startIcon={<CancelIcon />}
              variant="outlined"
              onClick={() => setSensor(undefined)}
              color="secondary"
            >
              Cancel
            </Button>
            <Button
              startIcon={<SaveIcon />}
              variant="outlined"
              type="submit"
              onClick={() => sendSensor()}
              color="warning"
            >
              Save
            </Button>
          </DialogActions>
        </Dialog>
      );
    }
  };

  const renderDeviceDialog = () => {
    if (coreData && coreData.devices.length > 0 && deviceDialog !== -1) {
      return (
        <Dialog open={deviceDialog !== -1} onClose={() => setDeviceDialog(-1)}>
          <DialogTitle>Device Details</DialogTitle>
          <DialogContent dividers>
            <List dense={true}>
              <ListItem>
                <ListItemText primary="Type" secondary={coreData.devices[deviceDialog].t} />
              </ListItem>
              <ListItem>
                <ListItemText primary="Name" secondary={coreData.devices[deviceDialog].n} />
              </ListItem>
              <ListItem>
                <ListItemText primary="Brand" secondary={coreData.devices[deviceDialog].b} />
              </ListItem>
              <ListItem>
                <ListItemText
                  primary="Device ID"
                  secondary={'0x' + ('00' + coreData.devices[deviceDialog].d.toString(16).toUpperCase()).slice(-2)}
                />
              </ListItem>
              <ListItem>
                <ListItemText primary="Product ID" secondary={coreData.devices[deviceDialog].p} />
              </ListItem>
              <ListItem>
                <ListItemText primary="Version" secondary={coreData.devices[deviceDialog].v} />
              </ListItem>
            </List>
          </DialogContent>
          <DialogActions>
            <Button variant="outlined" onClick={() => setDeviceDialog(-1)} color="secondary">
              Close
            </Button>
          </DialogActions>
        </Dialog>
      );
    }
  };

  const renderCoreData = () => (
    <IconContext.Provider value={{ color: 'lightblue', size: '24', style: { verticalAlign: 'middle' } }}>
      {coreData.devices.length === 0 && <MessageBox my={2} level="warning" message="Scanning for EMS devices..." />}

      <Table data={{ nodes: coreData.devices }} select={device_select} theme={device_theme} layout={{ custom: true }}>
        {(tableList: any) => (
          <>
            <Header>
              <HeaderRow>
                <HeaderCell />
                <HeaderCell resize>TYPE</HeaderCell>
                <HeaderCell resize>DESCRIPTION</HeaderCell>
                <HeaderCell>ENTITIES</HeaderCell>
                <HeaderCell />
              </HeaderRow>
            </Header>
            <Body>
              {tableList.map((device: Device, index: number) => (
                <Row key={device.id} item={device}>
                  <Cell>
                    <DeviceIcon type={device.t} />
                  </Cell>
                  <Cell>{device.t}</Cell>
                  <Cell>{device.n}</Cell>
                  <Cell>{device.e}</Cell>
                  <Cell>
                    <IconButton size="small" onClick={() => setDeviceDialog(index)}>
                      <InfoOutlinedIcon color="info" sx={{ fontSize: 16, verticalAlign: 'middle' }} />
                    </IconButton>
                  </Cell>
                </Row>
              ))}
              {(coreData.active_sensors > 0 || coreData.analog_enabled) && (
                <Row key="sensor" item={{ id: 'sensor' }}>
                  <Cell>
                    <DeviceIcon type="Sensor" />
                  </Cell>
                  <Cell>Sensors</Cell>
                  <Cell>Attached EMS-ESP Sensors</Cell>
                  <Cell>{coreData.active_sensors}</Cell>
                  <Cell>
                    <IconButton size="small" onClick={() => addAnalogSensor()}>
                      <AddCircleOutlineOutlinedIcon sx={{ fontSize: 16, verticalAlign: 'middle' }} />
                    </IconButton>
                  </Cell>
                </Row>
              )}
            </Body>
          </>
        )}
      </Table>
    </IconContext.Provider>
  );

  const renderDeviceData = () => {
    if (!device_select.state.id || device_select.state.id === 'sensor') {
      return;
    }

    const sendCommand = (dv: DeviceValue) => {
      if (dv.c && me.admin && !hasMask(dv.id, DeviceEntityMask.DV_READONLY)) {
        setDeviceValue(dv);
      }
    };

    const renderNameCell = (dv: DeviceValue) => (
      <>
        {dv.id.slice(2)}&nbsp;
        {hasMask(dv.id, DeviceEntityMask.DV_FAVORITE) && <StarIcon color="primary" sx={{ fontSize: 12 }} />}
        {hasMask(dv.id, DeviceEntityMask.DV_READONLY) && <EditOffOutlinedIcon color="primary" sx={{ fontSize: 12 }} />}
        {hasMask(dv.id, DeviceEntityMask.DV_API_MQTT_EXCLUDE) && (
          <CommentsDisabledOutlinedIcon color="primary" sx={{ fontSize: 12 }} />
        )}
      </>
    );

    return (
      <>
        <Typography sx={{ pt: 2, pb: 1 }} variant="h6" color="primary">
          {deviceData.label}
        </Typography>

        <FormControlLabel
          control={<Checkbox size="small" name="onlyFav" checked={onlyFav} onChange={() => setOnlyFav(!onlyFav)} />}
          label={<span style={{ fontSize: '12px' }}>favorites only</span>}
        />

        <Table
          data={{
            nodes: onlyFav
              ? deviceData.data.filter((dv) => hasMask(dv.id, DeviceEntityMask.DV_FAVORITE))
              : deviceData.data
          }}
          theme={data_theme}
          sort={dv_sort}
        >
          {(tableList: any) => (
            <>
              <Header>
                <HeaderRow>
                  <HeaderCell resize>
                    <Button
                      fullWidth
                      style={{ fontSize: '14px', justifyContent: 'flex-start' }}
                      endIcon={getSortIcon(dv_sort.state, 'NAME')}
                      onClick={() => dv_sort.fns.onToggleSort({ sortKey: 'NAME' })}
                    >
                      ENTITY NAME
                    </Button>
                  </HeaderCell>
                  <HeaderCell>
                    <Button
                      fullWidth
                      style={{ fontSize: '14px', justifyContent: 'flex-start' }}
                      endIcon={getSortIcon(dv_sort.state, 'VALUE')}
                      onClick={() => dv_sort.fns.onToggleSort({ sortKey: 'VALUE' })}
                    >
                      VALUE
                    </Button>
                  </HeaderCell>
                  <HeaderCell />
                </HeaderRow>
              </Header>
              <Body>
                {tableList.map((dv: DeviceValue) => (
                  <Row key={dv.id} item={dv} onClick={() => sendCommand(dv)}>
                    <Cell>{renderNameCell(dv)}</Cell>
                    <Cell>{formatValue(dv.v, dv.u)}</Cell>
                    <Cell>
                      {dv.c && me.admin && !hasMask(dv.id, DeviceEntityMask.DV_READONLY) && (
                        <IconButton size="small" onClick={() => sendCommand(dv)}>
                          {isCmdOnly(dv) ? (
                            <PlayArrowIcon color="primary" sx={{ fontSize: 16 }} />
                          ) : (
                            <EditIcon color="primary" sx={{ fontSize: 16 }} />
                          )}
                        </IconButton>
                      )}
                    </Cell>
                  </Row>
                ))}
              </Body>
            </>
          )}
        </Table>
      </>
    );
  };

  const updateSensor = (s: Sensor) => {
    if (s && me.admin) {
      setSensor(s);
    }
  };

  const updateAnalog = (a: Analog) => {
    if (me.admin) {
      setAnalog(a);
    }
  };

  const renderDallasData = () => (
    <>
      <Typography sx={{ pt: 2, pb: 1 }} variant="h6" color="primary">
        Temperature Sensors
      </Typography>

      <Table data={{ nodes: sensorData.sensors }} theme={data_theme} sort={sensor_sort}>
        {(tableList: any) => (
          <>
            <Header>
              <HeaderRow>
                <HeaderCell resize>
                  <Button
                    fullWidth
                    style={{ fontSize: '14px', justifyContent: 'flex-start' }}
                    endIcon={getSortIcon(sensor_sort.state, 'NAME')}
                    onClick={() => sensor_sort.fns.onToggleSort({ sortKey: 'NAME' })}
                  >
                    NAME
                  </Button>
                </HeaderCell>
                <HeaderCell>
                  <Button
                    fullWidth
                    style={{ fontSize: '14px', justifyContent: 'flex-start' }}
                    endIcon={getSortIcon(sensor_sort.state, 'TEMPERATURE')}
                    onClick={() => sensor_sort.fns.onToggleSort({ sortKey: 'TEMPERATURE' })}
                  >
                    TEMPERATURE
                  </Button>
                </HeaderCell>
                <HeaderCell />
              </HeaderRow>
            </Header>
            <Body>
              {tableList.map((s: Sensor) => (
                <Row key={s.id} item={s} onClick={() => updateSensor(s)}>
                  <Cell>{s.n}</Cell>
                  <Cell>{formatValue(s.t, s.u)}</Cell>
                  <Cell>
                    {me.admin && (
                      <IconButton onClick={() => updateSensor(s)}>
                        <EditIcon color="primary" sx={{ fontSize: 16 }} />
                      </IconButton>
                    )}
                  </Cell>
                </Row>
              ))}
            </Body>
          </>
        )}
      </Table>
    </>
  );

  const renderAnalogData = () => (
    <>
      <Typography sx={{ pt: 2, pb: 1 }} variant="h6" color="primary">
        Analog Sensors
      </Typography>

      <Table data={{ nodes: sensorData.analogs }} theme={data_theme} sort={analog_sort}>
        {(tableList: any) => (
          <>
            <Header>
              <HeaderRow>
                <HeaderCell resize>
                  <Button
                    fullWidth
                    style={{ fontSize: '14px', justifyContent: 'flex-start' }}
                    endIcon={getSortIcon(analog_sort.state, 'GPIO')}
                    onClick={() => analog_sort.fns.onToggleSort({ sortKey: 'GPIO' })}
                  >
                    GPIO
                  </Button>
                </HeaderCell>
                <HeaderCell resize>
                  <Button
                    fullWidth
                    style={{ fontSize: '14px', justifyContent: 'flex-start' }}
                    endIcon={getSortIcon(analog_sort.state, 'NAME')}
                    onClick={() => analog_sort.fns.onToggleSort({ sortKey: 'NAME' })}
                  >
                    NAME
                  </Button>
                </HeaderCell>
                <HeaderCell resize>
                  <Button
                    fullWidth
                    style={{ fontSize: '14px', justifyContent: 'flex-start' }}
                    endIcon={getSortIcon(analog_sort.state, 'TYPE')}
                    onClick={() => analog_sort.fns.onToggleSort({ sortKey: 'TYPE' })}
                  >
                    TYPE
                  </Button>
                </HeaderCell>
                <HeaderCell>
                  <Button
                    fullWidth
                    style={{ fontSize: '14px', justifyContent: 'flex-start' }}
                    endIcon={getSortIcon(analog_sort.state, 'VALUE')}
                    onClick={() => analog_sort.fns.onToggleSort({ sortKey: 'VALUE' })}
                  >
                    VALUE
                  </Button>
                </HeaderCell>
                <HeaderCell />
              </HeaderRow>
            </Header>
            <Body>
              {tableList.map((a: Analog) => (
                <Row key={a.id} item={a} onClick={() => updateAnalog(a)}>
                  <Cell>{a.g}</Cell>
                  <Cell>{a.n}</Cell>
                  <Cell>{AnalogTypeNames[a.t]} </Cell>
                  <Cell>{a.t ? formatValue(a.v, a.u) : ''}</Cell>
                  <Cell>
                    {me.admin && (
                      <IconButton onClick={() => updateAnalog(a)}>
                        <EditIcon color="primary" sx={{ fontSize: 16 }} />
                      </IconButton>
                    )}
                  </Cell>
                </Row>
              ))}
            </Body>
          </>
        )}
      </Table>
    </>
  );

  const sendRemoveAnalog = async () => {
    if (analog) {
      try {
        const response = await EMSESP.writeAnalog({
          gpio: analog.g,
          name: analog.n,
          offset: analog.o,
          factor: analog.f,
          uom: analog.u,
          type: -1
        });

        if (response.status === 204) {
          enqueueSnackbar('Analog deletion failed', { variant: 'error' });
        } else if (response.status === 403) {
          enqueueSnackbar('Access denied', { variant: 'error' });
        } else {
          enqueueSnackbar('Analog sensor removed', { variant: 'success' });
        }
      } catch (error: any) {
        enqueueSnackbar(extractErrorMessage(error, 'Problem updating analog sensor'), { variant: 'error' });
      } finally {
        setAnalog(undefined);
        fetchSensorData();
      }
    }
  };

  const sendAnalog = async () => {
    if (analog) {
      try {
        const response = await EMSESP.writeAnalog({
          gpio: analog.g,
          name: analog.n,
          offset: analog.o,
          factor: analog.f,
          uom: analog.u,
          type: analog.t
        });

        if (response.status === 204) {
          enqueueSnackbar('Analog sensor update failed', { variant: 'error' });
        } else if (response.status === 403) {
          enqueueSnackbar('Access denied', { variant: 'error' });
        } else {
          enqueueSnackbar('Analog sensor updated', { variant: 'success' });
        }
      } catch (error: any) {
        enqueueSnackbar(extractErrorMessage(error, 'Problem updating analog'), { variant: 'error' });
      } finally {
        setAnalog(undefined);
        fetchSensorData();
      }
    }
  };

  const renderAnalogDialog = () => {
    if (analog) {
      return (
        <Dialog open={analog !== undefined} onClose={() => setAnalog(undefined)}>
          <DialogTitle>Edit Analog Sensor</DialogTitle>
          <DialogContent dividers>
            <Grid container spacing={2}>
              <Grid item>
                <ValidatedTextField
                  name="g"
                  label="GPIO"
                  value={analog.g}
                  type="number"
                  variant="outlined"
                  autoFocus
                  onChange={updateValue(setAnalog)}
                />
              </Grid>
              <Grid item>
                <ValidatedTextField
                  name="n"
                  label="Name"
                  value={analog.n}
                  sx={{ width: '20ch' }}
                  variant="outlined"
                  onChange={updateValue(setAnalog)}
                />
              </Grid>
              <Grid item>
                <ValidatedTextField name="t" label="Type" value={analog.t} select onChange={updateValue(setAnalog)}>
                  {AnalogTypeNames.map((val, i) => (
                    <MenuItem key={i} value={i}>
                      {val}
                    </MenuItem>
                  ))}
                </ValidatedTextField>
              </Grid>
              {analog.t >= AnalogType.COUNTER && analog.t <= AnalogType.RATE && (
                <>
                  <Grid item>
                    <ValidatedTextField name="u" label="UoM" value={analog.u} select onChange={updateValue(setAnalog)}>
                      {DeviceValueUOM_s.map((val, i) => (
                        <MenuItem key={i} value={i}>
                          {val}
                        </MenuItem>
                      ))}
                    </ValidatedTextField>
                  </Grid>
                  {analog.t === AnalogType.ADC && (
                    <Grid item>
                      <ValidatedTextField
                        name="o"
                        label="Offset"
                        value={numberValue(analog.o)}
                        sx={{ width: '20ch' }}
                        type="number"
                        variant="outlined"
                        onChange={updateValue(setAnalog)}
                        inputProps={{ min: '0', max: '3300', step: '1' }}
                        InputProps={{
                          startAdornment: <InputAdornment position="start">mV</InputAdornment>
                        }}
                      />
                    </Grid>
                  )}
                  {analog.t === AnalogType.COUNTER && (
                    <Grid item>
                      <ValidatedTextField
                        name="o"
                        label="Start Value"
                        value={numberValue(analog.o)}
                        sx={{ width: '20ch' }}
                        type="number"
                        variant="outlined"
                        onChange={updateValue(setAnalog)}
                        inputProps={{ min: '0', step: '1' }}
                      />
                    </Grid>
                  )}
                  <Grid item>
                    <ValidatedTextField
                      name="f"
                      label="Factor"
                      value={numberValue(analog.f)}
                      sx={{ width: '20ch' }}
                      type="number"
                      variant="outlined"
                      onChange={updateValue(setAnalog)}
                      inputProps={{ min: '-100', max: '100', step: '0.1' }}
                    />
                  </Grid>
                </>
              )}
              {analog.t === AnalogType.DIGITAL_OUT && (analog.id === '25' || analog.id === '26') && (
                <>
                  <Grid item>
                    <ValidatedTextField
                      name="o"
                      label="DAC Value"
                      value={numberValue(analog.o)}
                      sx={{ width: '20ch' }}
                      type="number"
                      variant="outlined"
                      onChange={updateValue(setAnalog)}
                      inputProps={{ min: '0', max: '255', step: '1' }}
                    />
                  </Grid>
                </>
              )}
              {analog.t === AnalogType.DIGITAL_OUT && analog.id !== '25' && analog.id !== '26' && (
                <>
                  <Grid item>
                    <ValidatedTextField
                      name="o"
                      label="Value"
                      value={numberValue(analog.o)}
                      sx={{ width: '20ch' }}
                      type="number"
                      variant="outlined"
                      onChange={updateValue(setAnalog)}
                      inputProps={{ min: '0', max: '1', step: '1' }}
                    />
                  </Grid>
                </>
              )}
              {analog.t >= AnalogType.PWM_0 && (
                <>
                  <Grid item>
                    <ValidatedTextField
                      name="f"
                      label="Frequency"
                      value={numberValue(analog.f)}
                      sx={{ width: '20ch' }}
                      type="number"
                      variant="outlined"
                      onChange={updateValue(setAnalog)}
                      inputProps={{ min: '1', max: '5000', step: '1' }}
                      InputProps={{
                        startAdornment: <InputAdornment position="start">Hz</InputAdornment>
                      }}
                    />
                  </Grid>
                  <Grid item>
                    <ValidatedTextField
                      name="o"
                      label="Dutycycle"
                      value={numberValue(analog.o)}
                      sx={{ width: '20ch' }}
                      type="number"
                      variant="outlined"
                      onChange={updateValue(setAnalog)}
                      inputProps={{ min: '0', max: '100', step: '0.1' }}
                      InputProps={{
                        startAdornment: <InputAdornment position="start">%</InputAdornment>
                      }}
                    />
                  </Grid>
                </>
              )}
            </Grid>
            <Box color="warning.main" mt={2}>
              <Typography variant="body2">Warning: be careful when assigning a GPIO!</Typography>
            </Box>
          </DialogContent>
          <DialogActions>
            <Box flexGrow={1} sx={{ '& button': { mt: 0 } }}>
              <Button startIcon={<RemoveIcon />} variant="outlined" color="error" onClick={() => sendRemoveAnalog()}>
                Remove
              </Button>
            </Box>
            <Button
              startIcon={<CancelIcon />}
              variant="outlined"
              onClick={() => setAnalog(undefined)}
              color="secondary"
            >
              Cancel
            </Button>
            <Button
              startIcon={<SaveIcon />}
              variant="outlined"
              type="submit"
              onClick={() => sendAnalog()}
              color="warning"
            >
              Save
            </Button>
          </DialogActions>
        </Dialog>
      );
    }
  };

  return (
    <SectionContent title="Device and Sensor Data" titleGutter>
      {renderCoreData()}
      {renderDeviceData()}
      {renderDeviceDialog()}
      {sensorData.sensors.length !== 0 && renderDallasData()}
      {sensorData.analogs.length !== 0 && renderAnalogData()}
      {renderDeviceValueDialog()}
      {renderSensorDialog()}
      {renderAnalogDialog()}
      <ButtonRow>
        <Button startIcon={<RefreshIcon />} variant="outlined" color="secondary" onClick={refreshData}>
          Refresh
        </Button>
        {device_select.state.id && device_select.state.id !== 'sensor' && (
          <Button startIcon={<DownloadIcon />} variant="outlined" onClick={handleDownloadCsv}>
            Export
          </Button>
        )}
      </ButtonRow>
    </SectionContent>
  );
};

export default DashboardData;
