import { FC, useState, useEffect, useCallback } from 'react';

import {
  Button,
  Typography,
  Box,
  MenuItem,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  ToggleButton,
  ToggleButtonGroup,
  Tooltip,
  Grid,
  TextField,
  Link
} from '@mui/material';

import { Table } from '@table-library/react-table-library/table';
import { useTheme } from '@table-library/react-table-library/theme';
import { useSort, SortToggleType } from '@table-library/react-table-library/sort';
import { Header, HeaderRow, HeaderCell, Body, Row, Cell } from '@table-library/react-table-library/table';

import { useSnackbar } from 'notistack';

import SaveIcon from '@mui/icons-material/Save';
import CancelIcon from '@mui/icons-material/Cancel';

import SettingsBackupRestoreIcon from '@mui/icons-material/SettingsBackupRestore';
import KeyboardArrowUpOutlinedIcon from '@mui/icons-material/KeyboardArrowUpOutlined';
import KeyboardArrowDownOutlinedIcon from '@mui/icons-material/KeyboardArrowDownOutlined';
import UnfoldMoreOutlinedIcon from '@mui/icons-material/UnfoldMoreOutlined';
import SearchIcon from '@mui/icons-material/Search';
import FilterListIcon from '@mui/icons-material/FilterList';

import OptionIcon from './OptionIcon';

import { ButtonRow, FormLoader, ValidatedTextField, SectionContent } from '../components';

import * as EMSESP from './api';

import { extractErrorMessage } from '../utils';

import { DeviceShort, Devices, DeviceEntity, DeviceEntityMask } from './types';

import { useI18nContext } from '../i18n/i18n-react';

export const APIURL = window.location.origin + '/api/';

const SettingsCustomization: FC = () => {
  const { LL } = useI18nContext();

  const { enqueueSnackbar } = useSnackbar();

  const [deviceEntities, setDeviceEntities] = useState<DeviceEntity[]>([{ id: '', v: 0, n: '', m: 0, w: false }]);
  const [devices, setDevices] = useState<Devices>();
  const [errorMessage, setErrorMessage] = useState<string>();
  const [selectedDevice, setSelectedDevice] = useState<number>(-1);
  const [confirmReset, setConfirmReset] = useState<boolean>(false);
  const [selectedFilters, setSelectedFilters] = useState<number>(0);
  const [search, setSearch] = useState('');

  // eslint-disable-next-line
  const [masks, setMasks] = useState(() => ['']);

  const entities_theme = useTheme({
    Table: `
      --data-table-library_grid-template-columns: 120px repeat(1, minmax(0, 1fr)) 120px;
    `,
    BaseRow: `
      font-size: 14px;
      .td {
        height: 32px;
      }
    `,
    BaseCell: `
      &:last-of-type {
        text-align: right;
      }
    `,
    HeaderRow: `
      text-transform: uppercase;
      background-color: black;
      color: #90CAF9;

      .th {
        border-bottom: 1px solid #565656;
        font-weight: 500;
      }

      &:nth-of-type(1) .th {
        text-align: center;
      }
    `,
    Row: `
      background-color: #1e1e1e;
      position: relative;
    
      .td {
        border-top: 1px solid #565656;
        border-bottom: 1px solid #565656;
      }

      &.tr.tr-body.row-select.row-select-single-selected {
        background-color: #3d4752;
        color: white;
        font-weight: normal;
      }

      &:nth-of-type(odd) .td {
        background-color: #303030;
      }
    `,
    Cell: `
      &:nth-of-type(2) {
        padding: 8px;
      }
      &:last-of-type {
        padding-right: 8px;
      }
    `
  });

  const getSortIcon = (state: any, sortKey: any) => {
    if (state.sortKey === sortKey && state.reverse) {
      return <KeyboardArrowDownOutlinedIcon />;
    }
    if (state.sortKey === sortKey && !state.reverse) {
      return <KeyboardArrowUpOutlinedIcon />;
    }
    return <UnfoldMoreOutlinedIcon />;
  };

  const entity_sort = useSort(
    { nodes: deviceEntities },
    {},
    {
      sortIcon: {
        iconDefault: <UnfoldMoreOutlinedIcon />,
        iconUp: <KeyboardArrowUpOutlinedIcon />,
        iconDown: <KeyboardArrowDownOutlinedIcon />
      },
      sortToggleType: SortToggleType.AlternateWithReset,
      sortFns: {
        NAME: (array) => array.sort((a, b) => a.id.localeCompare(b.id))
      }
    }
  );

  const fetchDevices = useCallback(async () => {
    try {
      setDevices((await EMSESP.readDevices()).data);
    } catch (error: unknown) {
      setErrorMessage(extractErrorMessage(error, LL.PROBLEM_LOADING()));
    }
  }, [LL]);

  const setInitialMask = (data: DeviceEntity[]) => {
    setDeviceEntities(data.map((de) => ({ ...de, om: de.m })));
  };

  const fetchDeviceEntities = async (unique_id: number) => {
    try {
      const data = (await EMSESP.readDeviceEntities({ id: unique_id })).data;
      setInitialMask(data);
    } catch (error: unknown) {
      setErrorMessage(extractErrorMessage(error, LL.PROBLEM_LOADING()));
    }
  };

  useEffect(() => {
    fetchDevices();
  }, [fetchDevices]);

  function formatValue(value: any) {
    if (typeof value === 'number') {
      return new Intl.NumberFormat().format(value);
    } else if (value === undefined) {
      return '';
    } else if (typeof value === 'boolean') {
      return value ? 'true' : 'false';
    }
    return value;
  }

  function formatName(de: DeviceEntity) {
    if (de.n === undefined || de.n === de.id) {
      return de.id;
    } else if (de.n === '') {
      return LL.COMMAND() + ': ' + de.id;
    }
    return (
      <>
        {de.n}&nbsp;(
        <Link target="_blank" href={APIURL + devices?.devices[selectedDevice].t + '/' + de.id}>
          {de.id}
        </Link>
        )
      </>
    );
  }

  const getMaskNumber = (newMask: string[]) => {
    var new_mask = 0;
    for (let entry of newMask) {
      new_mask |= Number(entry);
    }
    return new_mask;
  };

  const getMaskString = (m: number) => {
    var new_masks = [];
    if ((m & 1) === 1) {
      new_masks.push('1');
    }
    if ((m & 2) === 2) {
      new_masks.push('2');
    }
    if ((m & 4) === 4) {
      new_masks.push('4');
    }
    if ((m & 8) === 8) {
      new_masks.push('8');
    }
    return new_masks;
  };

  const maskDisabled = (set: boolean) => {
    setDeviceEntities(
      deviceEntities.map(function (de) {
        if ((de.m & selectedFilters || !selectedFilters) && de.id.toLowerCase().includes(search.toLowerCase())) {
          return {
            ...de,
            m: set
              ? de.m | (DeviceEntityMask.DV_API_MQTT_EXCLUDE | DeviceEntityMask.DV_WEB_EXCLUDE)
              : de.m & ~(DeviceEntityMask.DV_API_MQTT_EXCLUDE | DeviceEntityMask.DV_WEB_EXCLUDE)
          };
        } else {
          return de;
        }
      })
    );
  };

  const changeSelectedDevice = (event: React.ChangeEvent<HTMLInputElement>) => {
    if (devices) {
      const selected_device = parseInt(event.target.value, 10);
      setSelectedDevice(selected_device);
      fetchDeviceEntities(devices?.devices[selected_device].i);
    }
  };

  const resetCustomization = async () => {
    try {
      await EMSESP.resetCustomizations();
      enqueueSnackbar(LL.CUSTOMIZATIONS_RESTART(), { variant: 'info' });
    } catch (error: unknown) {
      enqueueSnackbar(extractErrorMessage(error, LL.PROBLEM_UPDATING()), { variant: 'error' });
    } finally {
      setConfirmReset(false);
    }
  };

  const saveCustomization = async () => {
    if (devices && deviceEntities && selectedDevice !== -1) {
      const masked_entities = deviceEntities
        .filter((de) => de.m !== de.om)
        .map((new_de) => new_de.m.toString(16).padStart(2, '0') + new_de.id);

      if (masked_entities.length > 60) {
        enqueueSnackbar(LL.CUSTOMIZATIONS_FULL(), { variant: 'warning' });
        return;
      }

      try {
        const response = await EMSESP.writeMaskedEntities({
          id: devices?.devices[selectedDevice].i,
          entity_ids: masked_entities
        });
        if (response.status === 200) {
          enqueueSnackbar(LL.CUSTOMIZATIONS_SAVED(), { variant: 'success' });
        } else {
          enqueueSnackbar(LL.PROBLEM_UPDATING(), { variant: 'error' });
        }
      } catch (error: unknown) {
        enqueueSnackbar(extractErrorMessage(error, LL.PROBLEM_UPDATING()), { variant: 'error' });
      }
      setInitialMask(deviceEntities);
    }
  };

  const renderDeviceList = () => {
    if (!devices) {
      return <FormLoader errorMessage={errorMessage} />;
    }

    return (
      <>
        <Box mb={2} color="warning.main">
          <Typography variant="body2">{LL.CUSTOMIZATIONS_HELP_1()}:</Typography>
          <Typography variant="body2">
            <OptionIcon type="favorite" isSet={true} />={LL.CUSTOMIZATIONS_HELP_2()}&nbsp;&nbsp;
            <OptionIcon type="readonly" isSet={true} />={LL.CUSTOMIZATIONS_HELP_3()}&nbsp;&nbsp;
            <OptionIcon type="api_mqtt_exclude" isSet={true} />={LL.CUSTOMIZATIONS_HELP_4()}&nbsp;&nbsp;
            <OptionIcon type="web_exclude" isSet={true} />={LL.CUSTOMIZATIONS_HELP_5()}
          </Typography>
        </Box>
        <ValidatedTextField
          name="device"
          label={'EMS ' + LL.DEVICE()}
          variant="outlined"
          fullWidth
          value={selectedDevice}
          onChange={changeSelectedDevice}
          margin="normal"
          select
        >
          <MenuItem disabled key={0} value={-1}>
            {LL.SELECT_DEVICE()}...
          </MenuItem>
          {devices.devices.map((device: DeviceShort, index) => (
            <MenuItem key={index} value={index}>
              {device.s}
            </MenuItem>
          ))}
        </ValidatedTextField>
      </>
    );
  };

  const renderDeviceData = () => {
    if (devices?.devices.length === 0 || deviceEntities[0].id === '') {
      return;
    }

    const shown_data = deviceEntities.filter(
      (de) => (de.m & selectedFilters || !selectedFilters) && de.id.toLowerCase().includes(search.toLowerCase())
    );

    return (
      <>
        <Grid container mb={1} mt={0} spacing={1} direction="row" justifyContent="flex-start" alignItems="center">
          <Grid item>
            <Typography variant="subtitle2" color="primary">
              #:
            </Typography>
          </Grid>
          <Grid item>
            <Typography variant="subtitle2">
              {shown_data.length}/{deviceEntities.length}
            </Typography>
          </Grid>
          <Grid item>
            <SearchIcon color="primary" sx={{ fontSize: 16, verticalAlign: 'middle' }} />:
          </Grid>
          <Grid item xs={2}>
            <TextField
              size="small"
              variant="outlined"
              onChange={(event) => {
                setSearch(event.target.value);
              }}
            />
          </Grid>
          <Tooltip arrow placement="top" title="apply filter">
            <Grid item>
              <FilterListIcon color="primary" sx={{ fontSize: 14, verticalAlign: 'middle' }} />:
            </Grid>
          </Tooltip>
          <Grid item>
            <ToggleButtonGroup
              size="small"
              color="secondary"
              value={getMaskString(selectedFilters)}
              onChange={(event, mask) => {
                setSelectedFilters(getMaskNumber(mask));
              }}
            >
              <ToggleButton value="8">
                <OptionIcon type="favorite" isSet={true} />
              </ToggleButton>
              <ToggleButton value="4">
                <OptionIcon type="readonly" isSet={true} />
              </ToggleButton>
              <ToggleButton value="2">
                <OptionIcon type="api_mqtt_exclude" isSet={true} />
              </ToggleButton>
              <ToggleButton value="1">
                <OptionIcon type="web_exclude" isSet={true} />
              </ToggleButton>
            </ToggleButtonGroup>
          </Grid>

          <Grid item>
            <Tooltip arrow placement="top" title="set selected entities to be both visible and output">
              <Button
                size="small"
                sx={{ fontSize: 10 }}
                variant="outlined"
                color="inherit"
                onClick={() => maskDisabled(false)}
              >
                {LL.SET_ALL()}&nbsp;
                <OptionIcon type="api_mqtt_exclude" isSet={false} />
                <OptionIcon type="web_exclude" isSet={false} />
              </Button>
            </Tooltip>
          </Grid>
          <Grid item>
            <Tooltip arrow placement="top" title="set selected entities to be not visible and not output">
              <Button
                size="small"
                sx={{ fontSize: 10 }}
                variant="outlined"
                color="inherit"
                onClick={() => maskDisabled(true)}
              >
                {LL.SET_ALL()}&nbsp;
                <OptionIcon type="api_mqtt_exclude" isSet={true} />
                <OptionIcon type="web_exclude" isSet={true} />
              </Button>
            </Tooltip>
          </Grid>
        </Grid>
        <Table data={{ nodes: shown_data }} theme={entities_theme} sort={entity_sort} layout={{ custom: true }}>
          {(tableList: any) => (
            <>
              <Header>
                <HeaderRow>
                  <HeaderCell stiff>{LL.OPTIONS()}</HeaderCell>
                  <HeaderCell resize>
                    <Button
                      fullWidth
                      style={{ fontSize: '14px', justifyContent: 'flex-start' }}
                      endIcon={getSortIcon(entity_sort.state, 'NAME')}
                      onClick={() => entity_sort.fns.onToggleSort({ sortKey: 'NAME' })}
                    >
                      {LL.NAME()}
                    </Button>
                  </HeaderCell>
                  <HeaderCell resize>{LL.VALUE()}</HeaderCell>
                </HeaderRow>
              </Header>
              <Body>
                {tableList.map((de: DeviceEntity) => (
                  <Row key={de.id} item={de}>
                    <Cell stiff>
                      <ToggleButtonGroup
                        size="small"
                        color="secondary"
                        value={getMaskString(de.m)}
                        onChange={(event, mask) => {
                          de.m = getMaskNumber(mask);
                          if (de.n === '' && de.m & DeviceEntityMask.DV_READONLY) {
                            de.m = de.m | DeviceEntityMask.DV_WEB_EXCLUDE;
                          }
                          if (de.m & DeviceEntityMask.DV_WEB_EXCLUDE) {
                            de.m = de.m & ~DeviceEntityMask.DV_FAVORITE;
                          }
                          setMasks(['']);
                        }}
                      >
                        <ToggleButton value="8" disabled={(de.m & 1) !== 0 || de.n === undefined}>
                          <OptionIcon
                            type="favorite"
                            isSet={(de.m & DeviceEntityMask.DV_FAVORITE) === DeviceEntityMask.DV_FAVORITE}
                          />
                        </ToggleButton>
                        <ToggleButton value="4" disabled={!de.w || (de.m & 3) === 3}>
                          <OptionIcon
                            type="readonly"
                            isSet={(de.m & DeviceEntityMask.DV_READONLY) === DeviceEntityMask.DV_READONLY}
                          />
                        </ToggleButton>
                        <ToggleButton value="2" disabled={de.n === ''}>
                          <OptionIcon
                            type="api_mqtt_exclude"
                            isSet={
                              (de.m & DeviceEntityMask.DV_API_MQTT_EXCLUDE) === DeviceEntityMask.DV_API_MQTT_EXCLUDE
                            }
                          />
                        </ToggleButton>
                        <ToggleButton value="1" disabled={de.n === undefined}>
                          <OptionIcon
                            type="web_exclude"
                            isSet={(de.m & DeviceEntityMask.DV_WEB_EXCLUDE) === DeviceEntityMask.DV_WEB_EXCLUDE}
                          />
                        </ToggleButton>
                      </ToggleButtonGroup>
                    </Cell>
                    <Cell>{formatName(de)}</Cell>
                    <Cell>{formatValue(de.v)}</Cell>
                  </Row>
                ))}
              </Body>
            </>
          )}
        </Table>
      </>
    );
  };

  const renderResetDialog = () => (
    <Dialog open={confirmReset} onClose={() => setConfirmReset(false)}>
      <DialogTitle>{LL.RESET()}</DialogTitle>
      <DialogContent dividers>{LL.CUSTOMIZATIONS_RESET()}</DialogContent>
      <DialogActions>
        <Button startIcon={<CancelIcon />} variant="outlined" onClick={() => setConfirmReset(false)} color="secondary">
          {LL.CANCEL()}
        </Button>
        <Button
          startIcon={<SettingsBackupRestoreIcon />}
          variant="outlined"
          onClick={resetCustomization}
          autoFocus
          color="error"
        >
          {LL.RESET()}
        </Button>
      </DialogActions>
    </Dialog>
  );

  const content = () => {
    return (
      <>
        <Typography sx={{ pt: 2, pb: 2 }} variant="h6" color="primary">
          {LL.DEVICE_ENTITIES()}
        </Typography>
        {renderDeviceList()}
        {renderDeviceData()}
        <Box display="flex" flexWrap="wrap">
          <Box flexGrow={1}>
            <ButtonRow>
              <Button startIcon={<SaveIcon />} variant="outlined" color="primary" onClick={() => saveCustomization()}>
                {LL.SAVE()}
              </Button>
            </ButtonRow>
          </Box>
          <ButtonRow>
            <Button
              startIcon={<SettingsBackupRestoreIcon />}
              variant="outlined"
              color="error"
              onClick={() => setConfirmReset(true)}
            >
              {LL.RESET()}
            </Button>
          </ButtonRow>
        </Box>
        {renderResetDialog()}
      </>
    );
  };

  return (
    <SectionContent title={LL.USER_CUSTOMIZATION()} titleGutter>
      {content()}
    </SectionContent>
  );
};

export default SettingsCustomization;
