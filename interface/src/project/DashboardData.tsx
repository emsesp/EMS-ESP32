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

import { useTheme } from '@table-library/react-table-library/theme';
import { useSort, SortToggleType } from '@table-library/react-table-library/sort';
import { Table, Header, HeaderRow, HeaderCell, Body, Row, Cell } from '@table-library/react-table-library/table';
import { useRowSelect } from '@table-library/react-table-library/select';

import DownloadIcon from '@mui/icons-material/GetApp';
import RefreshIcon from '@mui/icons-material/Refresh';
import EditIcon from '@mui/icons-material/Edit';
import InfoOutlinedIcon from '@mui/icons-material/InfoOutlined';
import AddCircleOutlineOutlinedIcon from '@mui/icons-material/AddCircleOutlineOutlined';
import CancelIcon from '@mui/icons-material/Cancel';
import SendIcon from '@mui/icons-material/TrendingFlat';
import WarningIcon from '@mui/icons-material/Warning';
import RemoveIcon from '@mui/icons-material/RemoveCircleOutline';
import PlayArrowIcon from '@mui/icons-material/PlayArrow';
import EditOffOutlinedIcon from '@mui/icons-material/EditOffOutlined';
import CommentsDisabledOutlinedIcon from '@mui/icons-material/CommentsDisabledOutlined';
import KeyboardArrowUpOutlinedIcon from '@mui/icons-material/KeyboardArrowUpOutlined';
import KeyboardArrowDownOutlinedIcon from '@mui/icons-material/KeyboardArrowDownOutlined';
import UnfoldMoreOutlinedIcon from '@mui/icons-material/UnfoldMoreOutlined';
import StarIcon from '@mui/icons-material/Star';

import DeviceIcon from './DeviceIcon';

import { IconContext } from 'react-icons';

import { AuthenticatedContext } from 'contexts/authentication';

import { ButtonRow, ValidatedTextField, SectionContent, MessageBox } from 'components';

import * as EMSESP from './api';

import { numberValue, updateValue, extractErrorMessage } from 'utils';

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

import { useI18nContext } from 'i18n/i18n-react';

const DashboardData: FC = () => {
  const { me } = useContext(AuthenticatedContext);

  const { LL } = useI18nContext();

  const { enqueueSnackbar } = useSnackbar();

  const [coreData, setCoreData] = useState<CoreData>({
    connected: true,
    devices: [],
    s_n: '',
    active_sensors: 0,
    analog_enabled: false
  });

  const [deviceData, setDeviceData] = useState<DeviceData>({ label: '', data: [] });
  const [sensorData, setSensorData] = useState<SensorData>({ sensors: [], analogs: [] });
  const [deviceValue, setDeviceValue] = useState<DeviceValue>();
  const [sensor, setSensor] = useState<Sensor>();
  const [analog, setAnalog] = useState<Analog>();
  const [deviceDialog, setDeviceDialog] = useState<number>(-1);
  const [onlyFav, setOnlyFav] = useState(false);

  const common_theme = useTheme({
    BaseRow: `
      font-size: 14px;
    `,
    HeaderRow: `
      text-transform: uppercase;
      background-color: black;
      color: #90CAF9;

      .th {
        border-bottom: 1px solid #565656;
      }
    `,
    Row: `
      background-color: #1e1e1e;
      position: relative;
      cursor: pointer;
    
      .td {
        padding: 8px;
        border-top: 1px solid #565656;
        border-bottom: 1px solid #565656;
      }

      &.tr.tr-body.row-select.row-select-single-selected {
        background-color: #3d4752;
        color: white;
        font-weight: normal;
      }

      &:hover .td {
        border-top: 1px solid #177ac9;
        border-bottom: 1px solid #177ac9;
      }
    `,
    Cell: `
      &:last-of-type {
        text-align: right;
      },
    `
  });

  const device_theme = useTheme([
    common_theme,
    {
      Table: `
        --data-table-library_grid-template-columns: 40px 160px repeat(1, minmax(0, 1fr)) 100px 40px;
      `,
      BaseRow: `
        .td {
          height: 42px;
        }
      `,
      BaseCell: `
        &:nth-of-type(2) {
          text-align: left;
        },
        &:nth-of-type(4) {
          text-align: center;
        }
      `,
      HeaderRow: `
        .th {
          padding: 8px;
          height: 42px;
          font-weight: 500;
      `
    }
  ]);

  const data_theme = useTheme([
    common_theme,
    {
      Table: `
        --data-table-library_grid-template-columns: minmax(0, 1fr) 35% 40px;
      `,
      BaseRow: `
        .td {
          height: 32px;
        }
      `,
      BaseCell: `
        &:nth-of-type(2) {
          text-align: right;
        },
      `,
      HeaderRow: `
        .th {
          height: 32px;
        }
      `,
      Row: `
        &:nth-of-type(odd) .td {
          background-color: #303030;
        }
      `
    }
  ]);

  const temperature_theme = useTheme([data_theme]);

  const analog_theme = useTheme([
    data_theme,
    {
      Table: `
        --data-table-library_grid-template-columns: 80px repeat(1, minmax(0, 1fr)) 120px 100px 40px;
      `,
      BaseCell: `
        &:nth-of-type(2) {
          text-align: left;
        },
        &:nth-of-type(4) {
          text-align: right;
        }
      `
    }
  ]);

  const getSortIcon = (state: any, sortKey: any) => {
    if (state.sortKey === sortKey && state.reverse) {
      return <KeyboardArrowDownOutlinedIcon />;
    }
    if (state.sortKey === sortKey && !state.reverse) {
      return <KeyboardArrowUpOutlinedIcon />;
    }
    return <UnfoldMoreOutlinedIcon />;
  };

  const analog_sort = useSort(
    { nodes: sensorData.analogs },
    {},
    {
      sortIcon: {
        iconDefault: <UnfoldMoreOutlinedIcon />,
        iconUp: <KeyboardArrowUpOutlinedIcon />,
        iconDown: <KeyboardArrowDownOutlinedIcon />
      },
      sortToggleType: SortToggleType.AlternateWithReset,
      sortFns: {
        GPIO: (array) => array.sort((a, b) => a.g - b.g),
        NAME: (array) => array.sort((a, b) => a.n.localeCompare(b.n)),
        TYPE: (array) => array.sort((a, b) => a.t - b.t)
      }
    }
  );

  const sensor_sort = useSort(
    { nodes: sensorData.sensors },
    {},
    {
      sortIcon: {
        iconDefault: <UnfoldMoreOutlinedIcon />,
        iconUp: <KeyboardArrowUpOutlinedIcon />,
        iconDown: <KeyboardArrowDownOutlinedIcon />
      },
      sortToggleType: SortToggleType.AlternateWithReset,
      sortFns: {
        NAME: (array) => array.sort((a, b) => a.n.localeCompare(b.n)),
        TEMPERATURE: (array) => array.sort((a, b) => a.t - b.t)
      }
    }
  );

  const dv_sort = useSort(
    { nodes: deviceData.data },
    {},
    {
      sortIcon: {
        iconDefault: <UnfoldMoreOutlinedIcon />,
        iconUp: <KeyboardArrowUpOutlinedIcon />,
        iconDown: <KeyboardArrowDownOutlinedIcon />
      },
      sortToggleType: SortToggleType.AlternateWithReset,
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
    if (sc.includes('"') || sc.includes(';') || sc.includes('\n') || sc.includes('\r')) {
      return '"' + sc.replace(/"/g, '""') + '"';
    }
    return sc;
  };

  const makeCsvData = (columns: any, data: any) => {
    return data.reduce((csvString: any, rowItem: any) => {
      return csvString + columns.map(({ accessor }: any) => escapeCsvCell(accessor(rowItem))).join(';') + '\r\n';
    }, columns.map(({ name }: any) => escapeCsvCell(name)).join(';') + '\r\n');
  };

  const downloadAsCsv = (columns: any, data: any, filename: string) => {
    const csvData = makeCsvData(columns, data);
    const csvFile = new Blob([csvData], { type: 'text/csv;charset:utf-8' });
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
      { accessor: (dv: any) => dv.id.slice(2), name: LL.ENTITY_NAME() },
      {
        accessor: (dv: any) => (typeof dv.v === 'number' ? new Intl.NumberFormat().format(dv.v) : dv.v),
        name: LL.VALUE(0)
      },
      { accessor: (dv: any) => DeviceValueUOM_s[dv.u], name: 'UoM' }
    ];
    downloadAsCsv(
      columns,
      onlyFav ? deviceData.data.filter((dv) => hasMask(dv.id, DeviceEntityMask.DV_FAVORITE)) : deviceData.data,
      'device_entities'
    );
  };

  const refreshDataIndex = (selectedDevice: string) => {
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

  const refreshData = () => {
    refreshDataIndex(device_select.state.id);
  };

  const fetchCoreData = useCallback(async () => {
    try {
      setCoreData((await EMSESP.readCoreData()).data);
    } catch (error) {
      enqueueSnackbar(extractErrorMessage(error, LL.PROBLEM_LOADING()), { variant: 'error' });
    }
  }, [enqueueSnackbar, LL]);

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
    } catch (error) {
      enqueueSnackbar(extractErrorMessage(error, LL.PROBLEM_LOADING()), { variant: 'error' });
    }
  };

  const fetchSensorData = async () => {
    try {
      setSensorData((await EMSESP.readSensorData()).data);
    } catch (error) {
      enqueueSnackbar(extractErrorMessage(error, LL.PROBLEM_LOADING()), { variant: 'error' });
    }
  };

  const isCmdOnly = (dv: DeviceValue) => dv.v === '' && dv.c;

  const formatDurationMin = (duration_min: number) => {
    const days = Math.trunc((duration_min * 60000) / 86400000);
    const hours = Math.trunc((duration_min * 60000) / 3600000) % 24;
    const minutes = Math.trunc((duration_min * 60000) / 60000) % 60;

    let formatted = '';
    if (days) {
      formatted += LL.NUM_DAYS({ num: days }) + ' ';
    }
    if (hours) {
      formatted += LL.NUM_HOURS({ num: hours }) + ' ';
    }
    if (minutes) {
      formatted += LL.NUM_MINUTES({ num: minutes });
    }
    return formatted;
  };

  function formatValue(value: any, uom: number) {
    if (value === undefined) {
      return '';
    }
    switch (uom) {
      case DeviceValueUOM.HOURS:
        return value ? formatDurationMin(value * 60) : LL.NUM_HOURS({ num: 0 });
      case DeviceValueUOM.MINUTES:
        return value ? formatDurationMin(value) : LL.NUM_MINUTES({ num: 0 });
      case DeviceValueUOM.SECONDS:
        return LL.NUM_SECONDS({ num: value });
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
      default:
        return new Intl.NumberFormat().format(value) + ' ' + DeviceValueUOM_s[uom];
    }
  }

  const setUom = (uom: number) => {
    switch (uom) {
      case DeviceValueUOM.HOURS:
        return LL.HOURS();
      case DeviceValueUOM.MINUTES:
        return LL.MINUTES();
      case DeviceValueUOM.SECONDS:
        return LL.SECONDS();
      default:
        return DeviceValueUOM_s[uom];
    }
  };

  const sendDeviceValue = async () => {
    if (deviceValue) {
      try {
        const response = await EMSESP.writeValue({
          id: Number(device_select.state.id),
          devicevalue: deviceValue
        });
        if (response.status === 204) {
          enqueueSnackbar(LL.WRITE_CMD_FAILED(), { variant: 'error' });
        } else if (response.status === 403) {
          enqueueSnackbar(LL.ACCESS_DENIED(), { variant: 'error' });
        } else {
          enqueueSnackbar(LL.WRITE_CMD_SENT(), { variant: 'success' });
        }
        setDeviceValue(undefined);
      } catch (error) {
        enqueueSnackbar(extractErrorMessage(error, LL.PROBLEM_UPDATING()), { variant: 'error' });
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
          <DialogTitle>{isCmdOnly(deviceValue) ? LL.RUN_COMMAND() : LL.CHANGE_VALUE()}</DialogTitle>
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
                  <MenuItem value={val} key={val}>
                    {val}
                  </MenuItem>
                ))}
              </ValidatedTextField>
            )}
            {!deviceValue.l && (
              <ValidatedTextField
                name="v"
                label={deviceValue.id.slice(2)}
                value={typeof deviceValue.v === 'number' ? Math.round(deviceValue.v * 10) / 10 : deviceValue.v}
                autoFocus
                multiline={deviceValue.u ? false : true}
                sx={{ width: '30ch' }}
                type={deviceValue.u ? 'number' : 'text'}
                onChange={updateValue(setDeviceValue)}
                inputProps={deviceValue.u ? { min: deviceValue.m, max: deviceValue.x, step: deviceValue.s } : {}}
                InputProps={{
                  startAdornment: <InputAdornment position="start">{setUom(deviceValue.u)}</InputAdornment>
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
              {LL.CANCEL()}
            </Button>
            <Button
              startIcon={<SendIcon />}
              variant="outlined"
              type="submit"
              onClick={() => sendDeviceValue()}
              color="warning"
            >
              {LL.SEND()}
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
          enqueueSnackbar(LL.UPLOAD_OF(LL.SENSOR()) + ' ' + LL.FAILED(), { variant: 'error' });
        } else if (response.status === 403) {
          enqueueSnackbar(LL.ACCESS_DENIED(), { variant: 'error' });
        } else {
          enqueueSnackbar(LL.UPDATED_OF(LL.SENSOR()), { variant: 'success' });
        }
        setSensor(undefined);
      } catch (error) {
        enqueueSnackbar(extractErrorMessage(error, LL.PROBLEM_UPDATING()), { variant: 'error' });
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
          <DialogTitle>
            {LL.EDIT()} {LL.TEMP_SENSOR()}
          </DialogTitle>
          <DialogContent dividers>
            <Box color="warning.main" p={0} pl={0} pr={0} mt={0} mb={2}>
              <Typography variant="body2">
                {LL.ID_OF(LL.SENSOR())}: {sensor.id}
              </Typography>
            </Box>
            <Grid container spacing={1}>
              <Grid item>
                <ValidatedTextField
                  name="n"
                  label={LL.ENTITY_NAME()}
                  value={sensor.n}
                  autoFocus
                  sx={{ width: '30ch' }}
                  onChange={updateValue(setSensor)}
                />
              </Grid>
              <Grid item>
                <ValidatedTextField
                  name="o"
                  label={LL.OFFSET()}
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
              {LL.CANCEL()}
            </Button>
            <Button
              startIcon={<WarningIcon color="warning" />}
              variant="contained"
              type="submit"
              onClick={() => sendSensor()}
              color="info"
            >
              {LL.UPDATE()}
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
          <DialogTitle>{LL.DEVICE_DETAILS()}</DialogTitle>
          <DialogContent dividers>
            <List dense={true}>
              <ListItem>
                <ListItemText primary={LL.TYPE()} secondary={coreData.devices[deviceDialog].tn} />
              </ListItem>
              <ListItem>
                <ListItemText primary={LL.NAME(0)} secondary={coreData.devices[deviceDialog].n} />
              </ListItem>
              <ListItem>
                <ListItemText primary={LL.BRAND()} secondary={coreData.devices[deviceDialog].b} />
              </ListItem>
              <ListItem>
                <ListItemText
                  primary={LL.ID_OF(LL.DEVICE())}
                  secondary={'0x' + ('00' + coreData.devices[deviceDialog].d.toString(16).toUpperCase()).slice(-2)}
                />
              </ListItem>
              <ListItem>
                <ListItemText primary={LL.ID_OF(LL.PRODUCT())} secondary={coreData.devices[deviceDialog].p} />
              </ListItem>
              <ListItem>
                <ListItemText primary={LL.VERSION()} secondary={coreData.devices[deviceDialog].v} />
              </ListItem>
            </List>
          </DialogContent>
          <DialogActions>
            <Button variant="outlined" onClick={() => setDeviceDialog(-1)} color="secondary">
              {LL.CLOSE()}
            </Button>
          </DialogActions>
        </Dialog>
      );
    }
  };

  const renderCoreData = () => (
    <IconContext.Provider value={{ color: 'lightblue', size: '24', style: { verticalAlign: 'middle' } }}>
      {!coreData.connected && <MessageBox my={2} level="error" message={LL.EMS_BUS_WARNING()} />}
      {coreData.connected && coreData.devices.length === 0 && (
        <MessageBox my={2} level="warning" message={LL.EMS_BUS_SCANNING()} />
      )}

      <Table data={{ nodes: coreData.devices }} select={device_select} theme={device_theme} layout={{ custom: true }}>
        {(tableList: any) => (
          <>
            <Header>
              <HeaderRow>
                <HeaderCell stiff />
                <HeaderCell stiff>{LL.TYPE()}</HeaderCell>
                <HeaderCell resize>{LL.DESCRIPTION()}</HeaderCell>
                <HeaderCell stiff>{LL.ENTITIES()}</HeaderCell>
                <HeaderCell stiff />
              </HeaderRow>
            </Header>
            <Body>
              {tableList.map((device: Device, index: number) => (
                <Row key={device.id} item={device}>
                  <Cell stiff>
                    <DeviceIcon type_id={device.t} />
                  </Cell>
                  <Cell stiff>{device.tn}</Cell>
                  <Cell>{device.n}</Cell>
                  <Cell stiff>{device.e}</Cell>
                  <Cell stiff>
                    <IconButton size="small" onClick={() => setDeviceDialog(index)}>
                      <InfoOutlinedIcon color="info" sx={{ fontSize: 16, verticalAlign: 'middle' }} />
                    </IconButton>
                  </Cell>
                </Row>
              ))}
              {(coreData.active_sensors > 0 || coreData.analog_enabled) && (
                <Row key="sensor" item={{ id: 'sensor' }}>
                  <Cell>
                    <DeviceIcon type_id={1} />
                  </Cell>
                  <Cell>{coreData.s_n}</Cell>
                  <Cell>{LL.ATTACHED_SENSORS()}</Cell>
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
        <Typography sx={{ pt: 2, pb: 1 }} variant="h6" color="secondary">
          {deviceData.label}
        </Typography>
        <FormControlLabel
          control={<Checkbox size="small" name="onlyFav" checked={onlyFav} onChange={() => setOnlyFav(!onlyFav)} />}
          label={
            <span style={{ fontSize: '12px' }}>
              {LL.SHOW_FAV()}&nbsp;
              <StarIcon color="primary" sx={{ fontSize: 12 }} />
            </span>
          }
        />
        <Table
          data={{
            nodes: onlyFav
              ? deviceData.data.filter((dv) => hasMask(dv.id, DeviceEntityMask.DV_FAVORITE))
              : deviceData.data
          }}
          theme={data_theme}
          sort={dv_sort}
          layout={{ custom: true }}
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
                      {LL.ENTITY_NAME()}
                    </Button>
                  </HeaderCell>
                  <HeaderCell resize>
                    <Button
                      fullWidth
                      style={{ fontSize: '14px', justifyContent: 'flex-end' }}
                      endIcon={getSortIcon(dv_sort.state, 'VALUE')}
                      onClick={() => dv_sort.fns.onToggleSort({ sortKey: 'VALUE' })}
                    >
                      {LL.VALUE(0)}
                    </Button>
                  </HeaderCell>
                  <HeaderCell stiff />
                </HeaderRow>
              </Header>
              <Body>
                {tableList.map((dv: DeviceValue) => (
                  <Row key={dv.id} item={dv} onClick={() => sendCommand(dv)}>
                    <Cell>{renderNameCell(dv)}</Cell>
                    <Cell>{formatValue(dv.v, dv.u)}</Cell>
                    <Cell stiff>
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
      <Typography sx={{ pt: 2, pb: 1 }} variant="h6" color="secondary">
        {LL.TEMP_SENSORS()}
      </Typography>
      <Table
        data={{ nodes: sensorData.sensors }}
        theme={temperature_theme}
        sort={sensor_sort}
        layout={{ custom: true }}
      >
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
                    {LL.ENTITY_NAME()}
                  </Button>
                </HeaderCell>
                <HeaderCell stiff>
                  <Button
                    fullWidth
                    style={{ fontSize: '14px', justifyContent: 'flex-end' }}
                    endIcon={getSortIcon(sensor_sort.state, 'TEMPERATURE')}
                    onClick={() => sensor_sort.fns.onToggleSort({ sortKey: 'TEMPERATURE' })}
                  >
                    {LL.VALUE(0)}
                  </Button>
                </HeaderCell>
                <HeaderCell stiff />
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
      <Typography sx={{ pt: 2, pb: 1 }} variant="h6" color="secondary">
        {LL.ANALOG_SENSORS()}
      </Typography>

      <Table data={{ nodes: sensorData.analogs }} theme={analog_theme} sort={analog_sort} layout={{ custom: true }}>
        {(tableList: any) => (
          <>
            <Header>
              <HeaderRow>
                <HeaderCell stiff>
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
                    {LL.ENTITY_NAME()}
                  </Button>
                </HeaderCell>
                <HeaderCell stiff>
                  <Button
                    fullWidth
                    style={{ fontSize: '14px', justifyContent: 'flex-start' }}
                    endIcon={getSortIcon(analog_sort.state, 'TYPE')}
                    onClick={() => analog_sort.fns.onToggleSort({ sortKey: 'TYPE' })}
                  >
                    {LL.TYPE()}
                  </Button>
                </HeaderCell>
                <HeaderCell stiff>{LL.VALUE(0)}</HeaderCell>
                <HeaderCell stiff />
              </HeaderRow>
            </Header>
            <Body>
              {tableList.map((a: Analog) => (
                <Row key={a.id} item={a} onClick={() => updateAnalog(a)}>
                  <Cell stiff>{a.g}</Cell>
                  <Cell>{a.n}</Cell>
                  <Cell stiff>{AnalogTypeNames[a.t]} </Cell>
                  <Cell stiff>{a.t ? formatValue(a.v, a.u) : ''}</Cell>
                  <Cell stiff>
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
          enqueueSnackbar(LL.DELETION_OF(LL.ANALOG_SENSOR()) + ' ' + LL.FAILED(), { variant: 'error' });
        } else if (response.status === 403) {
          enqueueSnackbar(LL.ACCESS_DENIED(), { variant: 'error' });
        } else {
          enqueueSnackbar(LL.REMOVED_OF(LL.ANALOG_SENSOR()), { variant: 'success' });
        }
      } catch (error) {
        enqueueSnackbar(extractErrorMessage(error, LL.PROBLEM_UPDATING()), { variant: 'error' });
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
          enqueueSnackbar(LL.UPDATE_OF(LL.ANALOG_SENSOR()) + ' ' + LL.FAILED(), { variant: 'error' });
        } else if (response.status === 403) {
          enqueueSnackbar(LL.ACCESS_DENIED(), { variant: 'error' });
        } else {
          enqueueSnackbar(LL.UPDATED_OF(LL.ANALOG_SENSOR()), { variant: 'success' });
        }
      } catch (error) {
        enqueueSnackbar(extractErrorMessage(error, LL.PROBLEM_UPDATING()), { variant: 'error' });
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
          <DialogTitle>
            {LL.EDIT()} {LL.ANALOG_SENSOR()}
          </DialogTitle>
          <DialogContent dividers>
            <Grid container spacing={2}>
              <Grid item xs={12}>
                <ValidatedTextField
                  name="n"
                  label={LL.ENTITY_NAME()}
                  value={analog.n}
                  fullWidth
                  variant="outlined"
                  onChange={updateValue(setAnalog)}
                />
              </Grid>
              <Grid item xs={4}>
                <ValidatedTextField
                  name="g"
                  label="GPIO"
                  value={numberValue(analog.g)}
                  fullWidth
                  type="number"
                  variant="outlined"
                  autoFocus
                  onChange={updateValue(setAnalog)}
                />
              </Grid>
              <Grid item xs={8}>
                <ValidatedTextField
                  name="t"
                  label={LL.TYPE()}
                  value={analog.t}
                  fullWidth
                  select
                  onChange={updateValue(setAnalog)}
                >
                  {AnalogTypeNames.map((val, i) => (
                    <MenuItem key={i} value={i}>
                      {val}
                    </MenuItem>
                  ))}
                </ValidatedTextField>
              </Grid>
              {analog.t >= AnalogType.COUNTER && analog.t <= AnalogType.RATE && (
                <>
                  <Grid item xs={4}>
                    <ValidatedTextField
                      name="u"
                      label={LL.UNIT()}
                      value={analog.u}
                      fullWidth
                      select
                      onChange={updateValue(setAnalog)}
                    >
                      {DeviceValueUOM_s.map((val, i) => (
                        <MenuItem key={i} value={i}>
                          {val}
                        </MenuItem>
                      ))}
                    </ValidatedTextField>
                  </Grid>
                  {analog.t === AnalogType.ADC && (
                    <Grid item xs={4}>
                      <ValidatedTextField
                        name="o"
                        label={LL.OFFSET()}
                        value={numberValue(analog.o)}
                        fullWidth
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
                    <Grid item xs={4}>
                      <ValidatedTextField
                        name="o"
                        label={LL.STARTVALUE()}
                        value={numberValue(analog.o)}
                        fullWidth
                        type="number"
                        variant="outlined"
                        onChange={updateValue(setAnalog)}
                        inputProps={{ step: '0.001' }}
                      />
                    </Grid>
                  )}
                  <Grid item xs={4}>
                    <ValidatedTextField
                      name="f"
                      label={LL.FACTOR()}
                      value={numberValue(analog.f)}
                      fullWidth
                      type="number"
                      variant="outlined"
                      onChange={updateValue(setAnalog)}
                      inputProps={{ step: '0.001' }}
                    />
                  </Grid>
                </>
              )}
              {analog.t === AnalogType.DIGITAL_OUT && (analog.g === 25 || analog.g === 26) && (
                <>
                  <Grid item xs={4}>
                    <ValidatedTextField
                      name="o"
                      label={LL.VALUE(0)}
                      value={numberValue(analog.o)}
                      fullWidth
                      type="number"
                      variant="outlined"
                      onChange={updateValue(setAnalog)}
                      inputProps={{ min: '0', max: '255', step: '1' }}
                    />
                  </Grid>
                </>
              )}
              {analog.t === AnalogType.DIGITAL_OUT && analog.g !== 25 && analog.g !== 26 && (
                <>
                  <Grid item xs={4}>
                    <ValidatedTextField
                      name="o"
                      label={LL.VALUE(0)}
                      value={numberValue(analog.o)}
                      fullWidth
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
                  <Grid item xs={4}>
                    <ValidatedTextField
                      name="f"
                      label={LL.FREQ()}
                      value={numberValue(analog.f)}
                      fullWidth
                      type="number"
                      variant="outlined"
                      onChange={updateValue(setAnalog)}
                      inputProps={{ min: '1', max: '5000', step: '1' }}
                      InputProps={{
                        startAdornment: <InputAdornment position="start">Hz</InputAdornment>
                      }}
                    />
                  </Grid>
                  <Grid item xs={4}>
                    <ValidatedTextField
                      name="o"
                      label={LL.DUTY_CYCLE()}
                      value={numberValue(analog.o)}
                      fullWidth
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
              <Typography variant="body2">{LL.WARN_GPIO()}</Typography>
            </Box>
          </DialogContent>
          <DialogActions>
            <Box flexGrow={1} sx={{ '& button': { mt: 0 } }}>
              <Button startIcon={<RemoveIcon />} variant="outlined" color="error" onClick={() => sendRemoveAnalog()}>
                {LL.REMOVE()}
              </Button>
            </Box>
            <Button
              startIcon={<CancelIcon />}
              variant="outlined"
              onClick={() => setAnalog(undefined)}
              color="secondary"
            >
              {LL.CANCEL()}
            </Button>
            <Button
              startIcon={<WarningIcon color="warning" />}
              variant="contained"
              type="submit"
              onClick={() => sendAnalog()}
              color="info"
            >
              {LL.UPDATE()}
            </Button>
          </DialogActions>
        </Dialog>
      );
    }
  };

  return (
    <SectionContent title={LL.DEVICE_SENSOR_DATA()} titleGutter>
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
          {LL.REFRESH()}
        </Button>
        {device_select.state.id && device_select.state.id !== 'sensor' && (
          <Button startIcon={<DownloadIcon />} variant="outlined" onClick={handleDownloadCsv}>
            {LL.EXPORT()}
          </Button>
        )}
      </ButtonRow>
    </SectionContent>
  );
};

export default DashboardData;
