import CommentsDisabledOutlinedIcon from '@mui/icons-material/CommentsDisabledOutlined';
import EditIcon from '@mui/icons-material/Edit';
import EditOffOutlinedIcon from '@mui/icons-material/EditOffOutlined';
import DownloadIcon from '@mui/icons-material/GetApp';
import InfoOutlinedIcon from '@mui/icons-material/InfoOutlined';
import KeyboardArrowDownOutlinedIcon from '@mui/icons-material/KeyboardArrowDownOutlined';
import KeyboardArrowUpOutlinedIcon from '@mui/icons-material/KeyboardArrowUpOutlined';
import PlayArrowIcon from '@mui/icons-material/PlayArrow';
import RefreshIcon from '@mui/icons-material/Refresh';
import StarIcon from '@mui/icons-material/Star';
import UnfoldMoreOutlinedIcon from '@mui/icons-material/UnfoldMoreOutlined';
import {
  Button,
  Typography,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  IconButton,
  List,
  ListItem,
  ListItemText,
  FormControlLabel,
  Checkbox
} from '@mui/material';
import { useRowSelect } from '@table-library/react-table-library/select';
import { useSort, SortToggleType } from '@table-library/react-table-library/sort';
import { Table, Header, HeaderRow, HeaderCell, Body, Row, Cell } from '@table-library/react-table-library/table';
import { useTheme } from '@table-library/react-table-library/theme';
import { useState, useContext, useCallback, useEffect } from 'react';

import { IconContext } from 'react-icons';
import { toast } from 'react-toastify';
import DashboarDevicesDialog from './DashboardDevicesDialog';
import DeviceIcon from './DeviceIcon';

import * as EMSESP from './api';

import { DeviceValueUOM, DeviceValueUOM_s, DeviceEntityMask } from './types';
import type { Device, CoreData, DeviceData, DeviceValue } from './types';
import type { FC } from 'react';
import { ButtonRow, SectionContent, MessageBox } from 'components';
import { AuthenticatedContext } from 'contexts/authentication';

import { useI18nContext } from 'i18n/i18n-react';
import { extractErrorMessage } from 'utils';

const DashboardDevices: FC = () => {
  const { me } = useContext(AuthenticatedContext);
  const { LL } = useI18nContext();
  const [deviceData, setDeviceData] = useState<DeviceData>({ label: '', data: [] });
  const [selectedDeviceValue, setSelectedDeviceValue] = useState<DeviceValue>();
  const [deviceDetails, setDeviceDetails] = useState<number>(-1);
  const [onlyFav, setOnlyFav] = useState(false);
  const [selectedDevice, setSelectedDevice] = useState<number>();
  const [deviceValueDialogOpen, setDeviceValueDialogOpen] = useState(false);

  const [coreData, setCoreData] = useState<CoreData>({
    connected: true,
    devices: []
  });

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
          height: 36px;
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
          height: 36px;
        }
      `,
      Row: `
        &:nth-of-type(odd) .td {
          background-color: #303030;
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

  const fetchDeviceData = async (id: number) => {
    try {
      setDeviceData((await EMSESP.readDeviceData({ id })).data);
    } catch (error) {
      toast.error(extractErrorMessage(error, LL.PROBLEM_LOADING()));
    }
  };

  const fetchCoreData = useCallback(async () => {
    try {
      setCoreData((await EMSESP.readCoreData()).data);
    } catch (error) {
      toast.error(extractErrorMessage(error, LL.PROBLEM_LOADING()));
    }
  }, [LL]);

  useEffect(() => {
    void fetchCoreData();
  }, []);

  const refreshData = () => {
    if (selectedDevice) {
      void fetchDeviceData(selectedDevice);
    } else {
      void fetchCoreData();
    }
  };

  function onSelectChange(action: any, state: any) {
    setSelectedDevice(device_select.state.id);
    refreshData();
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

  const makeCsvData = (columns: any, data: any) =>
    data.reduce(
      (csvString: any, rowItem: any) =>
        csvString + columns.map(({ accessor }: any) => escapeCsvCell(accessor(rowItem))).join(';') + '\r\n',
      columns.map(({ name }: any) => escapeCsvCell(name)).join(';') + '\r\n'
    );

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

  useEffect(() => {
    const timer = setInterval(() => refreshData(), 60000);
    return () => {
      clearInterval(timer);
    };
  }, []);

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

  const deviceValueDialogSave = async (dv: DeviceValue) => {
    try {
      const response = await EMSESP.writeDeviceValue({
        id: Number(device_select.state.id),
        devicevalue: dv
      });
      if (response.status === 204) {
        toast.error(LL.WRITE_CMD_FAILED());
      } else if (response.status === 403) {
        toast.error(LL.ACCESS_DENIED());
      } else {
        toast.success(LL.WRITE_CMD_SENT());
      }
    } catch (error) {
      toast.error(extractErrorMessage(error, LL.PROBLEM_UPDATING()));
    } finally {
      setDeviceValueDialogOpen(false);
      setSelectedDeviceValue(undefined);
      refreshData();
    }
  };

  const renderDeviceDetails = () => {
    if (coreData && coreData.devices.length > 0 && deviceDetails !== -1) {
      return (
        <Dialog open={deviceDetails !== -1} onClose={() => setDeviceDetails(-1)}>
          <DialogTitle>{LL.DEVICE_DETAILS()}</DialogTitle>
          <DialogContent dividers>
            <List dense={true}>
              <ListItem>
                <ListItemText primary={LL.TYPE(0)} secondary={coreData.devices[deviceDetails].tn} />
              </ListItem>
              <ListItem>
                <ListItemText primary={LL.NAME(0)} secondary={coreData.devices[deviceDetails].n} />
              </ListItem>
              <ListItem>
                <ListItemText primary={LL.BRAND()} secondary={coreData.devices[deviceDetails].b} />
              </ListItem>
              <ListItem>
                <ListItemText
                  primary={LL.ID_OF(LL.DEVICE())}
                  secondary={'0x' + ('00' + coreData.devices[deviceDetails].d.toString(16).toUpperCase()).slice(-2)}
                />
              </ListItem>
              <ListItem>
                <ListItemText primary={LL.ID_OF(LL.PRODUCT())} secondary={coreData.devices[deviceDetails].p} />
              </ListItem>
              <ListItem>
                <ListItemText primary={LL.VERSION()} secondary={coreData.devices[deviceDetails].v} />
              </ListItem>
            </List>
          </DialogContent>
          <DialogActions>
            <Button variant="outlined" onClick={() => setDeviceDetails(-1)} color="secondary">
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
                <HeaderCell stiff>{LL.TYPE(0)}</HeaderCell>
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
                    <IconButton size="small" onClick={() => setDeviceDetails(index)}>
                      <InfoOutlinedIcon color="info" sx={{ fontSize: 16, verticalAlign: 'middle' }} />
                    </IconButton>
                  </Cell>
                </Row>
              ))}
            </Body>
          </>
        )}
      </Table>
    </IconContext.Provider>
  );

  const deviceValueDialogClose = () => {
    setDeviceValueDialogOpen(false);
  };

  const renderDeviceData = () => {
    if (!device_select.state.id) {
      return;
    }

    const sendCommand = (dv: DeviceValue) => {
      if (dv.c && me.admin && !hasMask(dv.id, DeviceEntityMask.DV_READONLY)) {
        setSelectedDeviceValue(dv);
        setDeviceValueDialogOpen(true);
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

  return (
    <SectionContent title={LL.DEVICE_DATA()} titleGutter>
      {renderCoreData()}
      {renderDeviceData()}
      {renderDeviceDetails()}
      {console.log('redndering device data')}

      {selectedDeviceValue && (
        <DashboarDevicesDialog
          open={deviceValueDialogOpen}
          onClose={deviceValueDialogClose}
          onSave={deviceValueDialogSave}
          selectedItem={selectedDeviceValue}
        />
      )}

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

export default DashboardDevices;
