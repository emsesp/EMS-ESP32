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

import { MessageBox } from '../components';
import PowerSettingsNewIcon from '@mui/icons-material/PowerSettingsNew';

import { Table } from '@table-library/react-table-library/table';
import { useTheme } from '@table-library/react-table-library/theme';
import { Header, HeaderRow, HeaderCell, Body, Row, Cell } from '@table-library/react-table-library/table';

import { useSnackbar } from 'notistack';

import SaveIcon from '@mui/icons-material/Save';
import CancelIcon from '@mui/icons-material/Cancel';

import SettingsBackupRestoreIcon from '@mui/icons-material/SettingsBackupRestore';
import SearchIcon from '@mui/icons-material/Search';
import FilterListIcon from '@mui/icons-material/FilterList';

import OptionIcon from './OptionIcon';

import { ButtonRow, FormLoader, ValidatedTextField, SectionContent } from '../components';

import * as EMSESP from './api';

import { extractErrorMessage, updateValue } from '../utils';

import { DeviceShort, Devices, DeviceEntity, DeviceEntityMask } from './types';

import { useI18nContext } from '../i18n/i18n-react';
import RestartMonitor from '../framework/system/RestartMonitor';

export const APIURL = window.location.origin + '/api/';

const SettingsCustomization: FC = () => {
  const { LL } = useI18nContext();
  const [restarting, setRestarting] = useState<boolean>(false);
  const [restartNeeded, setRestartNeeded] = useState<boolean>(false);

  const { enqueueSnackbar } = useSnackbar();

  const emptyDeviceEntity = { id: '', v: 0, n: '', cn: '', m: 0, w: false };

  const [deviceEntities, setDeviceEntities] = useState<DeviceEntity[]>([emptyDeviceEntity]);
  const [devices, setDevices] = useState<Devices>();
  const [errorMessage, setErrorMessage] = useState<string>();
  const [selectedDevice, setSelectedDevice] = useState<number>(-1);
  const [confirmReset, setConfirmReset] = useState<boolean>(false);
  const [selectedFilters, setSelectedFilters] = useState<number>(0);
  const [search, setSearch] = useState('');

  const [deviceEntity, setDeviceEntity] = useState<DeviceEntity>();

  // eslint-disable-next-line
  const [masks, setMasks] = useState(() => ['']);

  const entities_theme = useTheme({
    Table: `
      --data-table-library_grid-template-columns: 150px repeat(1, minmax(80px, 1fr)) 45px 45px 120px;
    `,
    BaseRow: `
      font-size: 14px;
      .td {
        height: 32px;
      }
    `,
    BaseCell: `
      &:nth-of-type(3) {
        text-align: right;
      }
      &:nth-of-type(4) {
        text-align: right;
      }
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
      cursor: pointer;
    
      .td {
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

      &:nth-of-type(odd) .td {
        background-color: #303030;
      }
    `,
    Cell: `
      &:nth-of-type(2) {
        padding: 8px;
      }
      &:nth-of-type(3) {
        padding-right: 4px;
      }
      &:nth-of-type(4) {
        padding-right: 4px;
      }
      &:last-of-type {
        padding-right: 8px;
      }
    `
  });

  const fetchDevices = useCallback(async () => {
    try {
      setDevices((await EMSESP.readDevices()).data);
    } catch (error) {
      setErrorMessage(extractErrorMessage(error, LL.PROBLEM_LOADING()));
    }
  }, [LL]);

  const setInitialMask = (data: DeviceEntity[]) => {
    setDeviceEntities(data.map((de) => ({ ...de, o_m: de.m, o_cn: de.cn, o_mi: de.mi, o_ma: de.ma })));
  };

  const fetchDeviceEntities = async (unique_id: number) => {
    try {
      const new_deviceEntities = (await EMSESP.readDeviceEntities({ id: unique_id })).data;
      setInitialMask(new_deviceEntities);
    } catch (error) {
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
    return (
      <>
        {de.n && (de.n[0] === '!' ? LL.COMMAND() + ': ' + de.n.slice(1) : de.cn && de.cn !== '' ? de.cn : de.n) + ' '}(
        <Link target="_blank" href={APIURL + devices?.devices[selectedDevice].tn + '/' + de.id}>
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
    if ((m & 128) === 128) {
      new_masks.push('128');
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
      setRestartNeeded(false);
    }
  };

  const resetCustomization = async () => {
    try {
      await EMSESP.resetCustomizations();
      enqueueSnackbar(LL.CUSTOMIZATIONS_RESTART(), { variant: 'info' });
    } catch (error) {
      enqueueSnackbar(extractErrorMessage(error, LL.PROBLEM_UPDATING()), { variant: 'error' });
    } finally {
      setConfirmReset(false);
    }
  };

  const restart = async () => {
    try {
      await EMSESP.restart();
      setRestarting(true);
    } catch (error) {
      enqueueSnackbar(extractErrorMessage(error, LL.PROBLEM_UPDATING()), { variant: 'error' });
    }
  };

  const saveCustomization = async () => {
    if (devices && deviceEntities && selectedDevice !== -1) {
      const masked_entities = deviceEntities
        .filter((de) => de.m !== de.o_m || de.cn !== de.o_cn || de.ma !== de.o_ma || de.mi !== de.o_mi)
        .map(
          (new_de) =>
            new_de.m.toString(16).padStart(2, '0') +
            new_de.id +
            (new_de.cn || new_de.mi || new_de.ma ? '|' : '') +
            (new_de.cn ? new_de.cn : '') +
            (new_de.mi ? '>' + new_de.mi : '') +
            (new_de.ma ? '<' + new_de.ma : '')
        );

      // check size in bytes to match buffer in CPP, which is 2048
      const bytes = new TextEncoder().encode(JSON.stringify(masked_entities)).length;
      if (bytes > 2000) {
        enqueueSnackbar(LL.CUSTOMIZATIONS_FULL(), { variant: 'warning' });
        return;
      }

      try {
        const response = await EMSESP.writeCustomEntities({
          id: devices?.devices[selectedDevice].i,
          entity_ids: masked_entities
        });
        if (response.status === 200) {
          enqueueSnackbar(LL.CUSTOMIZATIONS_SAVED(), { variant: 'success' });
        } else if (response.status === 201) {
          setRestartNeeded(true);
        } else {
          enqueueSnackbar(LL.PROBLEM_UPDATING(), { variant: 'error' });
        }
      } catch (error) {
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
          <Typography variant="body2">{LL.CUSTOMIZATIONS_HELP_1()}</Typography>
          <Typography variant="body2">
            <OptionIcon type="favorite" isSet={true} />={LL.CUSTOMIZATIONS_HELP_2()}&nbsp;&nbsp;
            <OptionIcon type="readonly" isSet={true} />={LL.CUSTOMIZATIONS_HELP_3()}&nbsp;&nbsp;
            <OptionIcon type="api_mqtt_exclude" isSet={true} />={LL.CUSTOMIZATIONS_HELP_4()}&nbsp;&nbsp;
            <OptionIcon type="web_exclude" isSet={true} />={LL.CUSTOMIZATIONS_HELP_5()}&nbsp;&nbsp;
            <OptionIcon type="deleted" isSet={true} />={LL.CUSTOMIZATIONS_HELP_6()}
          </Typography>
        </Box>
        <ValidatedTextField
          name="device"
          label={LL.EMS_DEVICE()}
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

  const editEntity = (de: DeviceEntity) => {
    if (de.n === undefined || (de.n && de.n[0] === '!')) {
      return;
    }

    if (de.cn === undefined) {
      de.cn = '';
    }
    setDeviceEntity(de);
  };

  const updateEntity = () => {
    if (deviceEntity) {
      setDeviceEntities((prevState) => {
        const newState = prevState.map((obj) => {
          if (obj.id === deviceEntity.id) {
            return { ...obj, cn: deviceEntity.cn, mi: deviceEntity.mi, ma: deviceEntity.ma };
          }
          return obj;
        });
        return newState;
      });
    }

    setDeviceEntity(undefined);
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
              <ToggleButton value="128">
                <OptionIcon type="deleted" isSet={true} />
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
        <Table data={{ nodes: shown_data }} theme={entities_theme} layout={{ custom: true }}>
          {(tableList: any) => (
            <>
              <Header>
                <HeaderRow>
                  <HeaderCell stiff>{LL.OPTIONS()}</HeaderCell>
                  <HeaderCell resize>
                    <Button fullWidth style={{ fontSize: '14px', justifyContent: 'flex-start' }}>
                      {LL.NAME(1)}
                    </Button>
                  </HeaderCell>
                  <HeaderCell stiff>{LL.MIN()}</HeaderCell>
                  <HeaderCell stiff>{LL.MAX()}</HeaderCell>
                  <HeaderCell resize>{LL.VALUE(0)}</HeaderCell>
                </HeaderRow>
              </Header>
              <Body>
                {tableList.map((de: DeviceEntity) => (
                  <Row key={de.id} item={de} onClick={() => editEntity(de)}>
                    <Cell stiff>
                      {!deviceEntity && (
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
                          <ToggleButton value="8" disabled={(de.m & 0x81) !== 0 || de.n === undefined}>
                            <OptionIcon
                              type="favorite"
                              isSet={(de.m & DeviceEntityMask.DV_FAVORITE) === DeviceEntityMask.DV_FAVORITE}
                            />
                          </ToggleButton>
                          <ToggleButton value="4" disabled={!de.w || (de.m & 0x83) >= 3}>
                            <OptionIcon
                              type="readonly"
                              isSet={(de.m & DeviceEntityMask.DV_READONLY) === DeviceEntityMask.DV_READONLY}
                            />
                          </ToggleButton>
                          <ToggleButton value="2" disabled={de.n === '' || (de.m & 0x80) !== 0}>
                            <OptionIcon
                              type="api_mqtt_exclude"
                              isSet={
                                (de.m & DeviceEntityMask.DV_API_MQTT_EXCLUDE) === DeviceEntityMask.DV_API_MQTT_EXCLUDE
                              }
                            />
                          </ToggleButton>
                          <ToggleButton value="1" disabled={de.n === undefined || (de.m & 0x80) !== 0}>
                            <OptionIcon
                              type="web_exclude"
                              isSet={(de.m & DeviceEntityMask.DV_WEB_EXCLUDE) === DeviceEntityMask.DV_WEB_EXCLUDE}
                            />
                          </ToggleButton>
                          <ToggleButton value="128">
                            <OptionIcon
                              type="deleted"
                              isSet={(de.m & DeviceEntityMask.DV_DELETED) === DeviceEntityMask.DV_DELETED}
                            />
                          </ToggleButton>
                        </ToggleButtonGroup>
                      )}
                    </Cell>
                    <Cell>{!deviceEntity && formatName(de)}</Cell>
                    <Cell>{!deviceEntity && !(de.m & DeviceEntityMask.DV_READONLY) && formatValue(de.mi)}</Cell>
                    <Cell>{!deviceEntity && !(de.m & DeviceEntityMask.DV_READONLY) && formatValue(de.ma)}</Cell>
                    <Cell>{!deviceEntity && formatValue(de.v)}</Cell>
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
      <DialogTitle>{LL.RESET(1)}</DialogTitle>
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
          {LL.RESET(0)}
        </Button>
      </DialogActions>
    </Dialog>
  );

  const renderContent = () => (
    <>
      <Typography sx={{ pt: 2, pb: 2 }} variant="h6" color="primary">
        {LL.DEVICE_ENTITIES()}
      </Typography>
      {renderDeviceList()}
      {renderDeviceData()}
      {restartNeeded && (
        <MessageBox my={2} level="warning" message={LL.RESTART_TEXT()}>
          <Button startIcon={<PowerSettingsNewIcon />} variant="contained" color="error" onClick={restart}>
            {LL.RESTART()}
          </Button>
        </MessageBox>
      )}
      {!restartNeeded && (
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
              {LL.RESET(0)}
            </Button>
          </ButtonRow>
        </Box>
      )}
      {renderResetDialog()}
    </>
  );

  const renderEditDialog = () => {
    if (deviceEntity) {
      const de = deviceEntity;
      return (
        <Dialog open={!!deviceEntity} onClose={() => setDeviceEntity(undefined)}>
          <DialogTitle>{LL.EDIT() + ' ' + LL.ENTITY() + ' "' + de.id + '"'}</DialogTitle>
          <DialogContent dividers>
            <Box color="warning.main" mb={2}>
              <Typography variant="body2">
                {LL.DEFAULT(1) + ' ' + LL.NAME(1)}:&nbsp;{deviceEntity.n}
              </Typography>
            </Box>
            <Grid container spacing={1}>
              <Grid item>
                <TextField
                  name="cn"
                  label={LL.NEW_NAME_OF(LL.ENTITY())}
                  value={deviceEntity.cn}
                  autoFocus
                  sx={{ width: '30ch' }}
                  onChange={updateValue(setDeviceEntity)}
                />
              </Grid>
              {typeof de.v === 'number' && de.w && !(de.m & DeviceEntityMask.DV_READONLY) && (
                <>
                  <Grid item>
                    <TextField
                      name="mi"
                      label={LL.MIN()}
                      value={deviceEntity.mi}
                      sx={{ width: '8ch' }}
                      onChange={updateValue(setDeviceEntity)}
                    />
                  </Grid>
                  <Grid item>
                    <TextField
                      name="ma"
                      label={LL.MAX()}
                      value={deviceEntity.ma}
                      sx={{ width: '8ch' }}
                      onChange={updateValue(setDeviceEntity)}
                    />
                  </Grid>
                </>
              )}
            </Grid>
          </DialogContent>
          <DialogActions>
            <Button
              startIcon={<CancelIcon />}
              variant="outlined"
              onClick={() => setDeviceEntity(undefined)}
              color="secondary"
            >
              {LL.CANCEL()}
            </Button>
            <Button
              startIcon={<SaveIcon />}
              variant="outlined"
              type="submit"
              onClick={() => updateEntity()}
              color="warning"
            >
              {LL.SAVE()}
            </Button>
          </DialogActions>
        </Dialog>
      );
    }
  };

  return (
    <SectionContent title={LL.USER_CUSTOMIZATION()} titleGutter>
      {restarting ? <RestartMonitor /> : renderContent()}
      {renderEditDialog()}
    </SectionContent>
  );
};

export default SettingsCustomization;
