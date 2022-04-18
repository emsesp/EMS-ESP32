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
  Grid
} from '@mui/material';

import { Table } from '@table-library/react-table-library/table';
import { useTheme } from '@table-library/react-table-library/theme';
import { useSort } from '@table-library/react-table-library/sort';
import { Header, HeaderRow, HeaderCell, Body, Row, Cell } from '@table-library/react-table-library/table';

import { useSnackbar } from 'notistack';

import SaveIcon from '@mui/icons-material/Save';
import CancelIcon from '@mui/icons-material/Cancel';
import EditOffOutlinedIcon from '@mui/icons-material/EditOffOutlined';
import StarIcon from '@mui/icons-material/Star';
import VisibilityOffOutlinedIcon from '@mui/icons-material/VisibilityOffOutlined';
import CommentsDisabledOutlinedIcon from '@mui/icons-material/CommentsDisabledOutlined';
import SettingsBackupRestoreIcon from '@mui/icons-material/SettingsBackupRestore';
import KeyboardArrowUpOutlinedIcon from '@mui/icons-material/KeyboardArrowUpOutlined';
import KeyboardArrowDownOutlinedIcon from '@mui/icons-material/KeyboardArrowDownOutlined';

import { ButtonRow, FormLoader, ValidatedTextField, SectionContent } from '../components';

import * as EMSESP from './api';

import { extractErrorMessage } from '../utils';

import { DeviceShort, Devices, DeviceEntity, DeviceEntityMask } from './types';

const SettingsCustomization: FC = () => {
  const { enqueueSnackbar } = useSnackbar();

  const [deviceEntities, setDeviceEntities] = useState<DeviceEntity[]>([{ id: '', v: 0, s: '', m: 0, w: false }]);
  const [devices, setDevices] = useState<Devices>();
  const [errorMessage, setErrorMessage] = useState<string>();
  const [selectedDevice, setSelectedDevice] = useState<number>(0);
  const [confirmReset, setConfirmReset] = useState<boolean>(false);
  const [selectedFilters, setSelectedFilters] = useState<number>(0);

  // eslint-disable-next-line
  const [masks, setMasks] = useState(() => ['']);

  const entities_theme = useTheme({
    BaseRow: `
      font-size: 14px;
      color: white;
      height: 32px;
    `,
    HeaderRow: `
      text-transform: uppercase;
      background-color: black;
      border-bottom: 1px solid #e0e0e0;
      color: #90CAF9;
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
        left: 0px;
        min-width: 124px;
        width: 124px;
      }
      &:nth-of-type(2) {
        min-width: 70%;
        width: 70%;
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
  };

  const entity_sort = useSort(
    { nodes: deviceEntities },
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
        NAME: (array) => array.sort((a, b) => a.id.localeCompare(b.id))
      }
    }
  );

  const fetchDevices = useCallback(async () => {
    try {
      setDevices((await EMSESP.readDevices()).data);
    } catch (error: any) {
      setErrorMessage(extractErrorMessage(error, 'Failed to fetch device list'));
    }
  }, []);

  const setInitialMask = (data: DeviceEntity[]) => {
    setDeviceEntities(data.map((de) => ({ ...de, om: de.m })));
  };

  const fetchDeviceEntities = async (unique_id: number) => {
    try {
      const data = (await EMSESP.readDeviceEntities({ id: unique_id })).data;
      setInitialMask(data);
    } catch (error: any) {
      setErrorMessage(extractErrorMessage(error, 'Problem fetching device entities'));
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

  const renderDeviceList = () => {
    if (!devices) {
      return <FormLoader errorMessage={errorMessage} />;
    }

    function compareDevices(a: DeviceShort, b: DeviceShort) {
      if (a.s < b.s) {
        return -1;
      }
      if (a.s > b.s) {
        return 1;
      }
      return 0;
    }

    const changeSelectedDevice = (event: React.ChangeEvent<HTMLInputElement>) => {
      const selected_device = parseInt(event.target.value, 10);
      setSelectedDevice(selected_device);
      fetchDeviceEntities(selected_device);
    };

    return (
      <>
        <Box color="warning.main">
          <Typography variant="body2">Select a device and customize each of its entities using the options.</Typography>
        </Box>
        <ValidatedTextField
          name="device"
          label="EMS Device"
          variant="outlined"
          fullWidth
          value={selectedDevice}
          onChange={changeSelectedDevice}
          margin="normal"
          select
        >
          <MenuItem disabled key={0} value={0}>
            Select a device...
          </MenuItem>
          {devices.devices.sort(compareDevices).map((device: DeviceShort, index) => (
            <MenuItem key={index} value={device.i}>
              {device.s}
            </MenuItem>
          ))}
        </ValidatedTextField>
      </>
    );
  };

  const saveCustomization = async () => {
    if (deviceEntities && selectedDevice) {
      const masked_entities = deviceEntities
        .filter((de) => de.m !== de.om)
        .map((new_de) => new_de.m.toString(16).padStart(2, '0') + new_de.s);

      if (masked_entities.length > 50) {
        enqueueSnackbar(
          'Too many selected entities (' + masked_entities.length + '). Limit is 50. Please Save in batches',
          { variant: 'warning' }
        );
        return;
      }

      try {
        const response = await EMSESP.writeMaskedEntities({
          id: selectedDevice,
          entity_ids: masked_entities
        });
        if (response.status === 200) {
          enqueueSnackbar('Customization saved', { variant: 'success' });
        } else {
          enqueueSnackbar('Customization save failed', { variant: 'error' });
        }
      } catch (error: any) {
        enqueueSnackbar(extractErrorMessage(error, 'Problem sending entity list'), { variant: 'error' });
      }
      setInitialMask(deviceEntities);
    }
  };

  const renderDeviceData = () => {
    if (devices?.devices.length === 0 || deviceEntities[0].id === '') {
      return;
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
        deviceEntities
          .filter((de) => de.m & selectedFilters || !selectedFilters)
          .map(({ m, ...entities }) => ({
            ...entities,
            m: set
              ? m | (DeviceEntityMask.DV_API_MQTT_EXCLUDE | DeviceEntityMask.DV_WEB_EXCLUDE)
              : m & ~(DeviceEntityMask.DV_API_MQTT_EXCLUDE | DeviceEntityMask.DV_WEB_EXCLUDE)
          }))
      );
    };

    return (
      <>
        <Grid
          container
          ml={1}
          mb={1}
          mt={1}
          spacing={1}
          direction="row"
          justifyContent="flex-start"
          alignItems="center"
        >
          <Grid item>
            <Typography align="right" variant="subtitle2" color="primary">
              Set filter:&nbsp;
            </Typography>
          </Grid>
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
                <Tooltip arrow placement="top" title="mark it as favorite to be listed at the top of the Dashboard">
                  <StarIcon sx={{ fontSize: 14 }} />
                </Tooltip>
              </ToggleButton>
              <ToggleButton value="4">
                <Tooltip arrow placement="top" title="make it read-only, only if it has write operation available">
                  <EditOffOutlinedIcon sx={{ fontSize: 14 }} />
                </Tooltip>
              </ToggleButton>
              <ToggleButton value="2">
                <Tooltip arrow placement="top" title="excluded it from MQTT and API outputs">
                  <CommentsDisabledOutlinedIcon sx={{ fontSize: 14 }} />
                </Tooltip>
              </ToggleButton>
              <ToggleButton value="1">
                <Tooltip arrow placement="top" title="hide it from the Dashboard">
                  <VisibilityOffOutlinedIcon sx={{ fontSize: 14 }} />
                </Tooltip>
              </ToggleButton>
            </ToggleButtonGroup>
          </Grid>
          <Grid item>
            <Button size="small" variant="contained" color="primary" onClick={() => maskDisabled(true)}>
              set all disabled
            </Button>
          </Grid>
          <Grid item>
            <Button size="small" variant="contained" color="primary" onClick={() => maskDisabled(false)}>
              set all enabled
            </Button>
          </Grid>
        </Grid>
        <Table
          data={{ nodes: deviceEntities.filter((de) => de.m & selectedFilters || !selectedFilters) }}
          theme={entities_theme}
          sort={entity_sort}
          layout={{ custom: true, horizontalScroll: true }}
        >
          {(tableList: any) => (
            <>
              <Header>
                <HeaderRow>
                  <HeaderCell pinLeft>OPTIONS</HeaderCell>
                  <HeaderCell resize>
                    <Button
                      fullWidth
                      style={{ fontSize: '14px', justifyContent: 'flex-start' }}
                      endIcon={getSortIcon(entity_sort.state, 'NAME')}
                      onClick={() => entity_sort.fns.onToggleSort({ sortKey: 'NAME' })}
                    >
                      NAME
                    </Button>
                  </HeaderCell>
                  <HeaderCell>VALUE</HeaderCell>
                  <HeaderCell />
                </HeaderRow>
              </Header>
              <Body>
                {tableList.map((de: DeviceEntity) => (
                  <Row key={de.id} item={de}>
                    <Cell>
                      <ToggleButtonGroup
                        size="small"
                        color="secondary"
                        value={getMaskString(de.m)}
                        onChange={(event, mask) => {
                          de.m = getMaskNumber(mask);
                          setMasks(['']);
                        }}
                      >
                        <ToggleButton value="8" disabled={(de.m & 1) !== 0 || de.id === ''}>
                          <StarIcon sx={{ fontSize: 14 }} />
                        </ToggleButton>
                        <ToggleButton value="4" disabled={!de.w}>
                          <EditOffOutlinedIcon sx={{ fontSize: 14 }} />
                        </ToggleButton>
                        <ToggleButton value="2">
                          <CommentsDisabledOutlinedIcon sx={{ fontSize: 14 }} />
                        </ToggleButton>
                        <ToggleButton value="1">
                          <VisibilityOffOutlinedIcon sx={{ fontSize: 14 }} />
                        </ToggleButton>
                      </ToggleButtonGroup>
                    </Cell>
                    <Cell>
                      {de.id}&nbsp;({de.s})
                    </Cell>
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

  const resetCustomization = async () => {
    try {
      await EMSESP.resetCustomizations();
      enqueueSnackbar('All customizations have been removed. Restarting...', { variant: 'info' });
    } catch (error: any) {
      enqueueSnackbar(extractErrorMessage(error, 'Problem resetting customizations'), { variant: 'error' });
    } finally {
      setConfirmReset(false);
    }
  };

  const renderResetDialog = () => (
    <Dialog open={confirmReset} onClose={() => setConfirmReset(false)}>
      <DialogTitle>Reset</DialogTitle>
      <DialogContent dividers>
        Are you sure you want remove all customizations including the custom settings of the Temperature and Analog
        sensors?
      </DialogContent>
      <DialogActions>
        <Button startIcon={<CancelIcon />} variant="outlined" onClick={() => setConfirmReset(false)} color="secondary">
          Cancel
        </Button>
        <Button
          startIcon={<SettingsBackupRestoreIcon />}
          variant="outlined"
          onClick={resetCustomization}
          autoFocus
          color="error"
        >
          Reset
        </Button>
      </DialogActions>
    </Dialog>
  );

  const content = () => {
    return (
      <>
        <Typography sx={{ pt: 2, pb: 2 }} variant="h6" color="primary">
          Device Entities
        </Typography>
        {renderDeviceList()}
        {renderDeviceData()}
        <Box display="flex" flexWrap="wrap">
          <Box flexGrow={1}>
            <ButtonRow>
              <Button startIcon={<SaveIcon />} variant="outlined" color="primary" onClick={() => saveCustomization()}>
                Save
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
              Reset
            </Button>
          </ButtonRow>
        </Box>
        {renderResetDialog()}
      </>
    );
  };

  return (
    <SectionContent title="User Customization" titleGutter>
      {content()}
    </SectionContent>
  );
};

export default SettingsCustomization;
