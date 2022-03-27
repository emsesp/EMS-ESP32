import { FC, useState, useEffect, useCallback } from 'react';

import {
  Button,
  Table,
  TableBody,
  TableHead,
  TableRow,
  Typography,
  Box,
  MenuItem,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  ToggleButton,
  ToggleButtonGroup
} from '@mui/material';

import TableCell, { tableCellClasses } from '@mui/material/TableCell';

import { styled } from '@mui/material/styles';

import { useSnackbar } from 'notistack';

import SaveIcon from '@mui/icons-material/Save';
import CancelIcon from '@mui/icons-material/Cancel';
import EditOffOutlinedIcon from '@mui/icons-material/EditOffOutlined';
import FavoriteBorderOutlinedIcon from '@mui/icons-material/FavoriteBorderOutlined';
import VisibilityOffOutlinedIcon from '@mui/icons-material/VisibilityOffOutlined';
import CommentsDisabledOutlinedIcon from '@mui/icons-material/CommentsDisabledOutlined';

import SettingsBackupRestoreIcon from '@mui/icons-material/SettingsBackupRestore';

import { ButtonRow, FormLoader, ValidatedTextField, SectionContent } from '../components';

import * as EMSESP from './api';

import { extractErrorMessage } from '../utils';

import { DeviceShort, Devices, DeviceEntity } from './types';

const StyledTableCell = styled(TableCell)(({ theme }) => ({
  [`&.${tableCellClasses.head}`]: {
    backgroundColor: '#607d8b',
    color: theme.palette.common.white,
    fontSize: 11
  },
  [`&.${tableCellClasses.body}`]: {
    fontSize: 11
  }
}));

const SettingsCustomization: FC = () => {
  const { enqueueSnackbar } = useSnackbar();

  const [deviceEntities, setDeviceEntities] = useState<DeviceEntity[]>();
  const [devices, setDevices] = useState<Devices>();
  const [errorMessage, setErrorMessage] = useState<string>();
  const [selectedDevice, setSelectedDevice] = useState<number>(0);
  const [confirmReset, setConfirmReset] = useState<boolean>(false);
  const [masks, setMasks] = useState(() => ['fav', 'readonly', 'exclude_mqtt', 'exclude_web']);

  const fetchDevices = useCallback(async () => {
    try {
      setDevices((await EMSESP.readDevices()).data);
    } catch (error: any) {
      setErrorMessage(extractErrorMessage(error, 'Failed to fetch device list'));
    }
  }, []);

  const fetchDeviceEntities = async (unique_id: number) => {
    try {
      setDeviceEntities((await EMSESP.readDeviceEntities({ id: unique_id })).data);
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
          <Typography variant="body2">
            Customize which entities to exclude. This will have immediate effect on all services including MQTT and API.
          </Typography>
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
        .filter((de) => de.m)
        .map((new_de) => new_de.m.toString(16).padStart(2, '0') + new_de.s);
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
    }
  };

  const renderDeviceData = () => {
    if (devices?.devices.length === 0 || !deviceEntities) {
      return;
    }

    const setMask = (de: DeviceEntity, newMask: string[]) => {
      var new_mask = 0;
      if (newMask.includes('exclude_web')) {
        new_mask |= 1;
      }
      if (newMask.includes('exclude_mqtt')) {
        new_mask |= 2;
      }
      if (newMask.includes('readonly')) {
        new_mask |= 4;
      }
      if (newMask.includes('fav')) {
        new_mask |= 8;
      }

      de.m = new_mask;
      setMasks(newMask);
    };

    const getValue = (de: DeviceEntity) => {
      var new_masks = [];
      if ((de.m & 1) === 1) {
        new_masks.push('exclude_web');
      }
      if ((de.m & 2) === 2) {
        new_masks.push('exclude_mqtt');
      }
      if ((de.m & 4) === 4) {
        new_masks.push('readonly');
      }
      if ((de.m & 8) === 8) {
        new_masks.push('fav');
      }

      return new_masks;
    };

    return (
      <Table size="small">
        <TableHead>
          <TableRow>
            <StyledTableCell align="center">OPTIONS</StyledTableCell>
            <StyledTableCell align="left">ENTITY NAME (CODE)</StyledTableCell>
            <StyledTableCell align="right">VALUE</StyledTableCell>
          </TableRow>
        </TableHead>
        <TableBody>
          {deviceEntities.map((de) => (
            <TableRow key={de.i}>
              <StyledTableCell padding="checkbox">
                <ToggleButtonGroup
                  size="small"
                  color="error"
                  value={getValue(de)}
                  onChange={(event, mask) => {
                    setMask(de, mask);
                  }}
                >
                  <ToggleButton value="fav" color="success">
                    <FavoriteBorderOutlinedIcon fontSize="small" />
                  </ToggleButton>
                  <ToggleButton value="readonly">
                    <EditOffOutlinedIcon fontSize="small" />
                  </ToggleButton>
                  <ToggleButton value="exclude_mqtt">
                    <CommentsDisabledOutlinedIcon fontSize="small" />
                  </ToggleButton>
                  <ToggleButton value="exclude_web">
                    <VisibilityOffOutlinedIcon fontSize="small" />
                  </ToggleButton>
                </ToggleButtonGroup>
              </StyledTableCell>
              <StyledTableCell>
                {de.n}&nbsp;({de.s})
              </StyledTableCell>
              <StyledTableCell align="right">{formatValue(de.v)}</StyledTableCell>
            </TableRow>
          ))}
        </TableBody>
      </Table>
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
        Are you sure you want remove all customizations? EMS-ESP will then restart.
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
