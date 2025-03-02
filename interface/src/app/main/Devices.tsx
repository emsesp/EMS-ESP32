import {
  useCallback,
  useContext,
  useEffect,
  useLayoutEffect,
  useState
} from 'react';
import { IconContext } from 'react-icons';
import { useNavigate } from 'react-router';
import { toast } from 'react-toastify';

import CommentsDisabledOutlinedIcon from '@mui/icons-material/CommentsDisabledOutlined';
import ConstructionIcon from '@mui/icons-material/Construction';
import EditIcon from '@mui/icons-material/Edit';
import EditOffOutlinedIcon from '@mui/icons-material/EditOffOutlined';
import DownloadIcon from '@mui/icons-material/GetApp';
import HighlightOffIcon from '@mui/icons-material/HighlightOff';
import InfoOutlinedIcon from '@mui/icons-material/InfoOutlined';
import KeyboardArrowDownOutlinedIcon from '@mui/icons-material/KeyboardArrowDownOutlined';
import KeyboardArrowUpOutlinedIcon from '@mui/icons-material/KeyboardArrowUpOutlined';
import PlayArrowIcon from '@mui/icons-material/PlayArrow';
import SearchIcon from '@mui/icons-material/Search';
import StarIcon from '@mui/icons-material/Star';
import StarBorderOutlinedIcon from '@mui/icons-material/StarBorderOutlined';
import UnfoldMoreOutlinedIcon from '@mui/icons-material/UnfoldMoreOutlined';
import {
  Box,
  Button,
  CircularProgress,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  Grid2 as Grid,
  IconButton,
  InputAdornment,
  List,
  ListItem,
  ListItemText,
  TextField,
  ToggleButton,
  Typography
} from '@mui/material';

import { useRowSelect } from '@table-library/react-table-library/select';
import { SortToggleType, useSort } from '@table-library/react-table-library/sort';
import {
  Body,
  Cell,
  Header,
  HeaderCell,
  HeaderRow,
  Row,
  Table
} from '@table-library/react-table-library/table';
import { useTheme } from '@table-library/react-table-library/theme';
import type { Action, State } from '@table-library/react-table-library/types/common';
import { dialogStyle } from 'CustomTheme';
import { useRequest } from 'alova/client';
import {
  ButtonTooltip,
  MessageBox,
  SectionContent,
  useLayoutTitle
} from 'components';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import { useInterval } from 'utils';

import { readCoreData, readDeviceData, writeDeviceValue } from '../../api/app';
import DeviceIcon from './DeviceIcon';
import DevicesDialog from './DevicesDialog';
import { formatValue } from './deviceValue';
import { DeviceEntityMask, DeviceType, DeviceValueUOM_s } from './types';
import type { Device, DeviceValue } from './types';
import { deviceValueItemValidation } from './validators';

const Devices = () => {
  const { LL } = useI18nContext();
  const { me } = useContext(AuthenticatedContext);

  const [size, setSize] = useState([0, 0]);
  const [selectedDeviceValue, setSelectedDeviceValue] = useState<DeviceValue>();
  const [onlyFav, setOnlyFav] = useState(false);
  const [deviceValueDialogOpen, setDeviceValueDialogOpen] = useState(false);
  const [showDeviceInfo, setShowDeviceInfo] = useState(false);
  const [selectedDevice, setSelectedDevice] = useState<number>();
  const [search, setSearch] = useState('');

  const navigate = useNavigate();

  useLayoutTitle(LL.DEVICES());

  const { data: coreData, send: sendCoreData } = useRequest(() => readCoreData(), {
    initialData: {
      connected: true,
      devices: []
    }
  });

  const { data: deviceData, send: sendDeviceData } = useRequest(
    (id: number) => readDeviceData(id),
    {
      initialData: {
        nodes: []
      },
      immediate: false
    }
  );

  const { loading: submitting, send: sendDeviceValue } = useRequest(
    (data: { id: number; c: string; v: unknown }) => writeDeviceValue(data),
    {
      immediate: false
    }
  );

  useLayoutEffect(() => {
    function updateSize() {
      setSize([window.innerWidth, window.innerHeight]);
    }
    window.addEventListener('resize', updateSize);
    updateSize();
    return () => window.removeEventListener('resize', updateSize);
  }, []);

  const leftOffset = () => {
    const devicesWindow = document.getElementById('devices-window');
    if (!devicesWindow) {
      return 0;
    }

    const clientRect = devicesWindow.getBoundingClientRect();
    const left = clientRect.left;
    const right = clientRect.right;

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
      cursor: pointer;
      background-color: #1E1E1E;
      .td {
        padding: 8px;
      }
      &.tr.tr-body.row-select.row-select-single-selected {
        background-color: #177ac9;
      }
    `
  });

  const device_theme = useTheme([
    common_theme,
    {
      Table: `
        --data-table-library_grid-template-columns: repeat(1, minmax(0, 1fr)) 130px;
      `,
      HeaderRow: `
        .th {
          padding: 8px;
      `,
      Row: `
        font-weight: bold;
        &:hover .td {
          background-color: #177ac9;
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
        },
        &:hover .td {
          background-color: #177ac9;
      }
      `
    }
  ]);

  const getSortIcon = (state: State, sortKey: unknown) => {
    if (state.sortKey === sortKey && state.reverse) {
      return <KeyboardArrowDownOutlinedIcon />;
    }
    if (state.sortKey === sortKey && !state.reverse) {
      return <KeyboardArrowUpOutlinedIcon />;
    }
    return <UnfoldMoreOutlinedIcon />;
  };

  const dv_sort = useSort(
    { nodes: deviceData.nodes },
    {},
    {
      sortIcon: {
        iconDefault: <UnfoldMoreOutlinedIcon />,
        iconUp: <KeyboardArrowUpOutlinedIcon />,
        iconDown: <KeyboardArrowDownOutlinedIcon />
      },
      sortToggleType: SortToggleType.AlternateWithReset,
      sortFns: {
        NAME: (array) =>
          array.sort((a, b) =>
            a.id.toString().slice(2).localeCompare(b.id.toString().slice(2))
          ),

        VALUE: (array) =>
          // eslint-disable-next-line @typescript-eslint/no-unsafe-return
          array.sort((a, b) => a.v.toString().localeCompare(b.v.toString()))
      }
    }
  );

  async function onSelectChange(action: Action, state: State) {
    setSelectedDevice(state.id as number);
    if (action.type === 'ADD_BY_ID_EXCLUSIVELY') {
      await sendDeviceData(state.id as number);
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
    setSearch('');
  };

  const escFunction = useCallback(
    (event: KeyboardEvent) => {
      if (event.key === 'Escape') {
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

  const customize = () => {
    if (selectedDevice === 99) {
      void navigate('/customentities');
    } else {
      void navigate('/customizations', { state: selectedDevice });
    }
  };

  const escapeCsvCell = (cell: string) => {
    if (cell == null) {
      return '';
    }
    const sc = cell.toString().trim();
    if (sc === '' || sc === '""') {
      return sc;
    }
    if (
      sc.includes('"') ||
      sc.includes(';') ||
      sc.includes('\n') ||
      sc.includes('\r')
    ) {
      return '"' + sc.replace(/"/g, '""') + '"';
    }
    return sc;
  };

  const hasMask = (id: string, mask: number) =>
    (parseInt(id.slice(0, 2), 16) & mask) === mask;

  const handleDownloadCsv = () => {
    const deviceIndex = coreData.devices.findIndex(
      (d) => d.id === device_select.state.id
    );
    if (deviceIndex === -1) {
      return;
    }
    const filename =
      coreData.devices[deviceIndex].tn + '_' + coreData.devices[deviceIndex].n;

    const columns = [
      {
        accessor: (dv: DeviceValue) => dv.id.slice(2),
        name: LL.ENTITY_NAME(0)
      },
      {
        accessor: (dv: DeviceValue) =>
          typeof dv.v === 'number' ? new Intl.NumberFormat().format(dv.v) : dv.v,
        name: LL.VALUE(0)
      },
      {
        accessor: (dv: DeviceValue) =>
          dv.u !== undefined && DeviceValueUOM_s[dv.u]
            ? DeviceValueUOM_s[dv.u].replace(/[^a-zA-Z0-9]/g, '')
            : '',
        name: 'UoM'
      },
      {
        accessor: (dv: DeviceValue) =>
          dv.c && !hasMask(dv.id, DeviceEntityMask.DV_READONLY) ? 'yes' : 'no',
        name: LL.WRITEABLE()
      },
      {
        accessor: (dv: DeviceValue) =>
          dv.h
            ? dv.h
            : dv.l
              ? dv.l.join(' | ')
              : dv.m !== undefined && dv.x !== undefined
                ? dv.m + ', ' + dv.x
                : '',
        name: 'Range'
      }
    ];

    const data = onlyFav
      ? deviceData.nodes.filter((dv) => hasMask(dv.id, DeviceEntityMask.DV_FAVORITE))
      : deviceData.nodes;

    const csvData = data.reduce(
      (csvString: string, rowItem: DeviceValue) =>
        csvString +
        columns
          .map(({ accessor }: { accessor: (dv: DeviceValue) => unknown }) =>
            escapeCsvCell(accessor(rowItem) as string)
          )
          .join(';') +
        '\r\n',
      columns.map(({ name }: { name: string }) => escapeCsvCell(name)).join(';') +
        '\r\n'
    );

    const downloadBlob = (blob: Blob) => {
      const downloadLink = document.createElement('a');
      downloadLink.download = filename;
      downloadLink.href = window.URL.createObjectURL(blob);
      document.body.appendChild(downloadLink);
      downloadLink.click();
      document.body.removeChild(downloadLink);
    };

    const device = { ...{ device: coreData.devices[deviceIndex] }, ...deviceData };
    downloadBlob(
      new Blob([JSON.stringify(device, null, 2)], {
        type: 'text;charset:utf-8'
      })
    );

    downloadBlob(new Blob([csvData], { type: 'text/csv;charset:utf-8' }));
  };

  useInterval(() => {
    if (!deviceValueDialogOpen) {
      selectedDevice ? void sendDeviceData(selectedDevice) : void sendCoreData();
    }
  });

  const deviceValueDialogSave = async (devicevalue: DeviceValue) => {
    const id = Number(device_select.state.id);
    await sendDeviceValue({ id, c: devicevalue.c ?? '', v: devicevalue.v })
      .then(() => {
        toast.success(LL.WRITE_CMD_SENT());
      })
      .catch((error: Error) => {
        toast.error(error.message);
      })
      .finally(async () => {
        setDeviceValueDialogOpen(false);
        await sendDeviceData(id);
        setSelectedDeviceValue(undefined);
      });
  };

  const renderDeviceDetails = () => {
    if (showDeviceInfo) {
      const deviceIndex = coreData.devices.findIndex(
        (d) => d.id === device_select.state.id
      );
      if (deviceIndex === -1) {
        return;
      }

      return (
        <Dialog
          sx={dialogStyle}
          open={showDeviceInfo}
          onClose={() => setShowDeviceInfo(false)}
        >
          <DialogTitle>{LL.DEVICE_DETAILS()}</DialogTitle>
          <DialogContent dividers>
            <List dense={true}>
              <ListItem>
                <ListItemText
                  primary={LL.TYPE(0)}
                  secondary={coreData.devices[deviceIndex].tn}
                />
              </ListItem>
              <ListItem>
                <ListItemText
                  primary={LL.NAME(0)}
                  secondary={coreData.devices[deviceIndex].n}
                />
              </ListItem>
              {coreData.devices[deviceIndex].t !== DeviceType.CUSTOM && (
                <>
                  <ListItem>
                    <ListItemText
                      primary={LL.BRAND()}
                      secondary={coreData.devices[deviceIndex].b}
                    />
                  </ListItem>
                  <ListItem>
                    <ListItemText
                      primary={LL.ID_OF(LL.DEVICE())}
                      secondary={
                        '0x' +
                        (
                          '00' +
                          coreData.devices[deviceIndex].d.toString(16).toUpperCase()
                        ).slice(-2)
                      }
                    />
                  </ListItem>
                  <ListItem>
                    <ListItemText
                      primary={LL.ID_OF(LL.PRODUCT())}
                      secondary={coreData.devices[deviceIndex].p}
                    />
                  </ListItem>
                  <ListItem>
                    <ListItemText
                      primary={LL.VERSION()}
                      secondary={coreData.devices[deviceIndex].v}
                    />
                  </ListItem>
                </>
              )}
            </List>
          </DialogContent>
          <DialogActions>
            <Button
              variant="outlined"
              onClick={() => setShowDeviceInfo(false)}
              color="secondary"
            >
              {LL.CLOSE()}
            </Button>
          </DialogActions>
        </Dialog>
      );
    }
  };

  const renderCoreData = () => (
    <>
      <IconContext.Provider
        value={{
          color: 'lightblue',
          size: '18',
          style: { verticalAlign: 'middle' }
        }}
      >
        {!coreData.connected && (
          <MessageBox my={2} level="error" message={LL.EMS_BUS_WARNING()} />
        )}

        {coreData.connected && (
          <Table
            data={{ nodes: coreData.devices }}
            select={device_select}
            theme={device_theme}
            layout={{ custom: true }}
          >
            {(tableList: Device[]) => (
              <>
                <Header>
                  <HeaderRow>
                    <HeaderCell resize>{LL.DESCRIPTION()}</HeaderCell>
                    <HeaderCell stiff>{LL.TYPE(0)}</HeaderCell>
                  </HeaderRow>
                </Header>
                <Body>
                  {tableList.length === 0 && (
                    <CircularProgress sx={{ margin: 1 }} size={18} />
                  )}
                  {tableList.map((device: Device) => (
                    <Row key={device.id} item={device}>
                      <Cell>
                        <DeviceIcon type_id={device.t} />
                        &nbsp;&nbsp;
                        {device.n}
                        <span style={{ color: 'lightblue' }}>
                          &nbsp;&nbsp;({device.e})
                        </span>
                      </Cell>
                      <Cell stiff>{device.tn}</Cell>
                    </Row>
                  ))}
                </Body>
              </>
            )}
          </Table>
        )}
      </IconContext.Provider>
    </>
  );

  const deviceValueDialogClose = () => {
    setDeviceValueDialogOpen(false);
    if (selectedDevice !== undefined) {
      void sendDeviceData(selectedDevice);
    }
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
        {hasMask(dv.id, DeviceEntityMask.DV_FAVORITE) && (
          <StarIcon color="primary" sx={{ fontSize: 12 }} />
        )}
        {hasMask(dv.id, DeviceEntityMask.DV_READONLY) && (
          <EditOffOutlinedIcon color="primary" sx={{ fontSize: 12 }} />
        )}
        {hasMask(dv.id, DeviceEntityMask.DV_API_MQTT_EXCLUDE) && (
          <CommentsDisabledOutlinedIcon color="primary" sx={{ fontSize: 12 }} />
        )}
      </>
    );

    const shown_data = onlyFav
      ? deviceData.nodes.filter(
          (dv) =>
            hasMask(dv.id, DeviceEntityMask.DV_FAVORITE) &&
            dv.id.slice(2).includes(search)
        )
      : deviceData.nodes.filter((dv) => dv.id.slice(2).includes(search));

    const deviceIndex = coreData.devices.findIndex(
      (d) => d.id === device_select.state.id
    );
    if (deviceIndex === -1) {
      return;
    }

    return (
      <Box
        sx={{
          backgroundColor: 'black',
          position: 'absolute',
          left: () => leftOffset(),
          right: 0,
          bottom: 0,
          top: 64,
          zIndex: 'modal',
          maxHeight: () => size[1] - 126,
          border: '1px solid #177ac9'
        }}
      >
        <Box sx={{ p: 1 }}>
          <Grid container justifyContent="space-between">
            <Typography noWrap variant="subtitle1" color="warning.main">
              {coreData.devices[deviceIndex].n}&nbsp;(
              {coreData.devices[deviceIndex].tn})
            </Typography>
            <Grid justifyContent="flex-end">
              <ButtonTooltip title={LL.CLOSE()}>
                <IconButton onClick={resetDeviceSelect}>
                  <HighlightOffIcon color="primary" sx={{ fontSize: 18 }} />
                </IconButton>
              </ButtonTooltip>
            </Grid>
          </Grid>

          <TextField
            size="small"
            variant="outlined"
            sx={{ width: '22ch' }}
            placeholder={LL.SEARCH()}
            onChange={(event) => {
              setSearch(event.target.value);
            }}
            slotProps={{
              input: {
                startAdornment: (
                  <InputAdornment position="start">
                    <SearchIcon color="primary" sx={{ fontSize: 16 }} />
                  </InputAdornment>
                )
              }
            }}
          />
          <ButtonTooltip title={LL.DEVICE_DETAILS()}>
            <IconButton onClick={() => setShowDeviceInfo(true)}>
              <InfoOutlinedIcon color="primary" sx={{ fontSize: 18 }} />
            </IconButton>
          </ButtonTooltip>
          {me.admin && (
            <ButtonTooltip title={LL.CUSTOMIZATIONS()}>
              <IconButton onClick={customize}>
                <ConstructionIcon color="primary" sx={{ fontSize: 18 }} />
              </IconButton>
            </ButtonTooltip>
          )}
          <ButtonTooltip title={LL.EXPORT()}>
            <IconButton onClick={handleDownloadCsv}>
              <DownloadIcon color="primary" sx={{ fontSize: 18 }} />
            </IconButton>
          </ButtonTooltip>

          <ButtonTooltip title={LL.FAVORITES()}>
            <ToggleButton
              value="1"
              size="small"
              selected={onlyFav}
              onChange={() => {
                setOnlyFav(!onlyFav);
              }}
            >
              {onlyFav ? (
                <StarIcon color="primary" sx={{ fontSize: 18 }} />
              ) : (
                <StarBorderOutlinedIcon color="primary" sx={{ fontSize: 18 }} />
              )}{' '}
            </ToggleButton>
          </ButtonTooltip>

          <span style={{ color: 'grey', fontSize: '12px' }}>
            &nbsp;
            {LL.SHOWING() +
              ' ' +
              shown_data.length +
              '/' +
              coreData.devices[deviceIndex].e +
              ' ' +
              LL.ENTITIES(shown_data.length)}
          </span>
        </Box>

        <Table
          data={{ nodes: shown_data }}
          theme={data_theme}
          sort={dv_sort}
          layout={{ custom: true, fixedHeader: true }}
        >
          {(tableList: DeviceValue[]) => (
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
                      {me.admin &&
                        dv.c &&
                        !hasMask(dv.id, DeviceEntityMask.DV_READONLY) && (
                          <IconButton
                            size="small"
                            onClick={() => showDeviceValue(dv)}
                          >
                            {dv.v === '' ? (
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
    <SectionContent id="devices-window">
      {renderCoreData()}
      {renderDeviceData()}
      {renderDeviceDetails()}
      {selectedDeviceValue && (
        <DevicesDialog
          open={deviceValueDialogOpen}
          onClose={deviceValueDialogClose}
          onSave={deviceValueDialogSave}
          selectedItem={selectedDeviceValue}
          writeable={
            selectedDeviceValue.c !== undefined &&
            !hasMask(selectedDeviceValue.id, DeviceEntityMask.DV_READONLY)
          }
          validator={deviceValueItemValidation(selectedDeviceValue)}
          progress={submitting}
        />
      )}
    </SectionContent>
  );
};

export default Devices;
