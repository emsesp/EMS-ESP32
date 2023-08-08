import CancelIcon from '@mui/icons-material/Cancel';
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
import StarBorderOutlinedIcon from '@mui/icons-material/StarBorderOutlined';
import UnfoldMoreOutlinedIcon from '@mui/icons-material/UnfoldMoreOutlined';

import {
  Button,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  IconButton,
  List,
  ListItem,
  ListItemText,
  Box,
  Grid,
  Typography
} from '@mui/material';
import { useRowSelect } from '@table-library/react-table-library/select';
import { useSort, SortToggleType } from '@table-library/react-table-library/sort';
import { Table, Header, HeaderRow, HeaderCell, Body, Row, Cell } from '@table-library/react-table-library/table';
import { useTheme } from '@table-library/react-table-library/theme';
import { useRequest } from 'alova';
import { useState, useContext, useEffect, useCallback, useLayoutEffect } from 'react';

import { IconContext } from 'react-icons';
import { toast } from 'react-toastify';
import DashboardDevicesDialog from './DashboardDevicesDialog';
import DeviceIcon from './DeviceIcon';

import * as EMSESP from './api';
import { formatValue } from './deviceValue';

import { DeviceValueUOM_s, DeviceEntityMask, DeviceType } from './types';
import { deviceValueItemValidation } from './validators';
import type { Device, DeviceValue } from './types';
import type { FC } from 'react';
import { dialogStyle } from 'CustomTheme';
import { ButtonRow, SectionContent, MessageBox } from 'components';
import { AuthenticatedContext } from 'contexts/authentication';

import { useI18nContext } from 'i18n/i18n-react';

const DashboardDevices: FC = () => {
  const [size, setSize] = useState([0, 0]);
  const { me } = useContext(AuthenticatedContext);
  const { LL } = useI18nContext();
  const [selectedDeviceValue, setSelectedDeviceValue] = useState<DeviceValue>();
  const [onlyFav, setOnlyFav] = useState(false);
  const [deviceValueDialogOpen, setDeviceValueDialogOpen] = useState(false);
  const [showDeviceInfo, setShowDeviceInfo] = useState<boolean>(false);
  const [selectedDevice, setSelectedDevice] = useState<number>();

  const { data: coreData, send: readCoreData } = useRequest(() => EMSESP.readCoreData(), {
    initialData: {
      connected: true,
      devices: []
    }
  });

  const { data: deviceData, send: readDeviceData } = useRequest((id) => EMSESP.readDeviceData(id), {
    initialData: {
      data: []
    },
    immediate: false
  });

  const { loading: submitting, send: writeDeviceValue } = useRequest((data) => EMSESP.writeDeviceValue(data), {
    immediate: false
  });

  useLayoutEffect(() => {
    function updateSize() {
      setSize([window.innerWidth, window.innerHeight]);
    }
    window.addEventListener('resize', updateSize);
    updateSize();
    return () => window.removeEventListener('resize', updateSize);
  }, []);

  const leftOffset = () => {
    const left = document.getElementById('devices-window')?.getBoundingClientRect().left;
    const right = document.getElementById('devices-window')?.getBoundingClientRect().right;

    if (!left || !right) {
      return 0;
    }

    return left + (right - left < 400 ? 0 : 200);
  };

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
      background-color: #1E1E1E;
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
    `
  });

  const device_theme = useTheme([
    common_theme,
    {
      Table: `
        --data-table-library_grid-template-columns: 40px repeat(1, minmax(0, 1fr)) 130px;
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
        --data-table-library_grid-template-columns: minmax(200px, auto) minmax(150px, auto) 40px;
        height: auto;
        max-height: 100%;
        overflow-y: scroll;
        ::-webkit-scrollbar {
          display:none;
        }
      `,
      BaseRow: `
      .td {
        height: 32px;
      }
     `,
      BaseCell: `
        &:nth-of-type(1) {
          border-left: 1px solid #177ac9;
        },
        &:nth-of-type(2) {
          text-align: right;
        },
        &:nth-of-type(3) {
          border-right: 1px solid #177ac9;
        }
      `,
      HeaderRow: `
        .th {
          border-top: 1px solid #565656;
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
        NAME: (array) => array.sort((a, b) => a.id.toString().slice(2).localeCompare(b.id.toString().slice(2))),
        VALUE: (array) => array.sort((a, b) => a.v.toString().localeCompare(b.v.toString()))
      }
    }
  );

  async function onSelectChange(action: any, state: any) {
    setSelectedDevice(state.id);
    if (action.type === 'ADD_BY_ID_EXCLUSIVELY') {
      await readDeviceData(state.id);
    }
  }

  const device_select = useRowSelect(
    { nodes: coreData.devices },
    {
      onChange: onSelectChange
    }
  );

  const resetDeviceSelect = () => {
    device_select.fns.onRemoveAll();
  };

  const escFunction = useCallback(
    (event: any) => {
      if (event.keyCode === 27) {
        if (device_select) {
          device_select.fns.onRemoveAll();
        }
      }
    },
    [device_select]
  );

  useEffect(() => {
    document.addEventListener('keydown', escFunction);
    return () => {
      document.removeEventListener('keydown', escFunction);
    };
  }, [escFunction]);

  const refreshData = () => {
    if (deviceValueDialogOpen) {
      return;
    }
    if (selectedDevice) {
      void readDeviceData(selectedDevice);
    } else {
      void readCoreData();
    }
  };

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
      { accessor: (dv: any) => dv.id.slice(2), name: LL.ENTITY_NAME(0) },
      {
        accessor: (dv: any) => (typeof dv.v === 'number' ? new Intl.NumberFormat().format(dv.v) : dv.v),
        name: LL.VALUE(0)
      },
      { accessor: (dv: any) => DeviceValueUOM_s[dv.u], name: 'UoM' }
    ];

    const deviceIndex = coreData.devices.findIndex((d) => d.id === device_select.state.id);
    if (deviceIndex === -1) {
      return;
    }
    const filename = coreData.devices[deviceIndex].tn + '_' + coreData.devices[deviceIndex].n;

    downloadAsCsv(
      columns,
      onlyFav ? deviceData.data.filter((dv) => hasMask(dv.id, DeviceEntityMask.DV_FAVORITE)) : deviceData.data,
      filename
    );
  };

  useEffect(() => {
    const timer = setInterval(() => refreshData(), 60000);
    return () => {
      clearInterval(timer);
    };
  });

  const deviceValueDialogSave = async (devicevalue: DeviceValue) => {
    const id = Number(device_select.state.id);
    await writeDeviceValue({ id, devicevalue })
      .then(() => {
        toast.success(LL.WRITE_CMD_SENT());
      })
      .catch((error) => {
        toast.error(error.message);
      })
      .finally(async () => {
        setDeviceValueDialogOpen(false);
        await readDeviceData(id);
        setSelectedDeviceValue(undefined);
      });
  };

  const renderDeviceDetails = () => {
    if (showDeviceInfo) {
      const deviceIndex = coreData.devices.findIndex((d) => d.id === device_select.state.id);
      if (deviceIndex === -1) {
        return;
      }

      return (
        <Dialog sx={dialogStyle} open={showDeviceInfo} onClose={() => setShowDeviceInfo(false)}>
          <DialogTitle>{LL.DEVICE_DETAILS()}</DialogTitle>
          <DialogContent dividers>
            <List dense={true}>
              <ListItem>
                <ListItemText primary={LL.TYPE(0)} secondary={coreData.devices[deviceIndex].tn} />
              </ListItem>
              <ListItem>
                <ListItemText primary={LL.NAME(0)} secondary={coreData.devices[deviceIndex].n} />
              </ListItem>
              {coreData.devices[deviceIndex].t !== DeviceType.CUSTOM && (
                <>
                  <ListItem>
                    <ListItemText primary={LL.BRAND()} secondary={coreData.devices[deviceIndex].b} />
                  </ListItem>
                  <ListItem>
                    <ListItemText
                      primary={LL.ID_OF(LL.DEVICE())}
                      secondary={'0x' + ('00' + coreData.devices[deviceIndex].d.toString(16).toUpperCase()).slice(-2)}
                    />
                  </ListItem>
                  <ListItem>
                    <ListItemText primary={LL.ID_OF(LL.PRODUCT())} secondary={coreData.devices[deviceIndex].p} />
                  </ListItem>
                  <ListItem>
                    <ListItemText primary={LL.VERSION()} secondary={coreData.devices[deviceIndex].v} />
                  </ListItem>
                </>
              )}
            </List>
          </DialogContent>
          <DialogActions>
            <Button variant="outlined" onClick={() => setShowDeviceInfo(false)} color="secondary">
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
                <HeaderCell resize>{LL.DESCRIPTION()}</HeaderCell>
                <HeaderCell stiff>{LL.TYPE(0)}</HeaderCell>
              </HeaderRow>
            </Header>
            <Body>
              {tableList.map((device: Device) => (
                <Row key={device.id} item={device}>
                  <Cell stiff>
                    <DeviceIcon type_id={device.t} />
                  </Cell>
                  <Cell>{device.n}</Cell>
                  <Cell stiff>{device.tn}</Cell>
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
    if (!selectedDevice) {
      return;
    }

    const showDeviceValue = (dv: DeviceValue) => {
      setSelectedDeviceValue(dv);
      setDeviceValueDialogOpen(true);
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

    const shown_data = onlyFav
      ? deviceData.data.filter((dv) => hasMask(dv.id, DeviceEntityMask.DV_FAVORITE))
      : deviceData.data;

    const deviceIndex = coreData.devices.findIndex((d) => d.id === device_select.state.id);
    if (deviceIndex === -1) {
      return;
    }

    return (
      <Box
        sx={{
          backgroundColor: 'black',
          position: 'absolute',
          left: () => leftOffset(),
          right: 16,
          bottom: 0,
          top: 128,
          maxHeight: () => size[1] - 210,
          zIndex: 'modal'
        }}
      >
        <Box sx={{ border: '1px solid #177ac9' }}>
          <Typography noWrap variant="subtitle1" color="warning.main" sx={{ mx: 1 }}>
            {coreData.devices[deviceIndex].n}
          </Typography>

          <Grid container justifyContent="space-between">
            <Typography sx={{ ml: 1 }} variant="subtitle2" color="primary">
              {shown_data.length + ' ' + LL.ENTITIES(shown_data.length)}
              <IconButton onClick={() => setShowDeviceInfo(true)}>
                <InfoOutlinedIcon color="primary" sx={{ fontSize: 18, verticalAlign: 'middle' }} />
              </IconButton>
              <IconButton onClick={handleDownloadCsv}>
                <DownloadIcon color="primary" sx={{ fontSize: 18, verticalAlign: 'middle' }} />
              </IconButton>
              <IconButton onClick={() => setOnlyFav(!onlyFav)}>
                {onlyFav ? (
                  <StarIcon color="primary" sx={{ fontSize: 18, verticalAlign: 'middle' }} />
                ) : (
                  <StarBorderOutlinedIcon color="primary" sx={{ fontSize: 18, verticalAlign: 'middle' }} />
                )}
              </IconButton>
              <IconButton onClick={refreshData}>
                <RefreshIcon color="primary" sx={{ fontSize: 18, verticalAlign: 'middle' }} />
              </IconButton>
            </Typography>
            <Grid item zeroMinWidth justifyContent="flex-end">
              <IconButton onClick={resetDeviceSelect}>
                <CancelIcon color="info" sx={{ fontSize: 18, verticalAlign: 'middle' }} />
              </IconButton>
            </Grid>
          </Grid>
        </Box>

        <Table
          data={{ nodes: shown_data }}
          theme={data_theme}
          sort={dv_sort}
          layout={{ custom: true, fixedHeader: true }}
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
                      {LL.ENTITY_NAME(0)}
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
                  <Row key={dv.id} item={dv} onClick={() => showDeviceValue(dv)}>
                    <Cell>{renderNameCell(dv)}</Cell>
                    <Cell>{formatValue(LL, dv.v, dv.u)}</Cell>
                    <Cell stiff>
                      {dv.c && me.admin && !hasMask(dv.id, DeviceEntityMask.DV_READONLY) && (
                        <IconButton size="small" onClick={() => showDeviceValue(dv)}>
                          {dv.v === '' && dv.c ? (
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
      </Box>
    );
  };

  return (
    <SectionContent title={LL.DEVICE_DATA()} titleGutter id="devices-window">
      {renderCoreData()}
      {renderDeviceData()}
      {renderDeviceDetails()}
      {selectedDeviceValue && (
        <DashboardDevicesDialog
          open={deviceValueDialogOpen}
          onClose={deviceValueDialogClose}
          onSave={deviceValueDialogSave}
          selectedItem={selectedDeviceValue}
          writeable={
            me.admin &&
            selectedDeviceValue.c !== undefined &&
            !hasMask(selectedDeviceValue.id, DeviceEntityMask.DV_READONLY)
          }
          validator={deviceValueItemValidation(selectedDeviceValue)}
          progress={submitting}
        />
      )}
      <ButtonRow>
        <Button startIcon={<RefreshIcon />} variant="outlined" color="secondary" onClick={refreshData}>
          {LL.REFRESH()}
        </Button>
      </ButtonRow>
    </SectionContent>
  );
};

export default DashboardDevices;
