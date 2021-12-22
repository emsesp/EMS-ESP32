import { FC, useState, useEffect, useCallback } from 'react';

import { Button, Table, TableBody, TableHead, TableRow, Typography, Box, MenuItem } from '@mui/material';

import TableCell, { tableCellClasses } from '@mui/material/TableCell';

import { styled } from '@mui/material/styles';

import { useSnackbar } from 'notistack';

import SaveIcon from '@mui/icons-material/Save';
import CloseIcon from '@mui/icons-material/Close';

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
            For each EMS device select which of its device entities are to be excluded. This will have immediate affect
            across all services. Note, entities that don't have a Value are automatically excluded.
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
          {devices.devices.sort(compareDevices).map((device: DeviceShort) => (
            <MenuItem key={device.i} value={device.i}>
              {device.s}
            </MenuItem>
          ))}
        </ValidatedTextField>
      </>
    );
  };

  const saveCustomization = async () => {
    if (deviceEntities && selectedDevice) {
      const exclude_entities = deviceEntities.filter((de) => de.x).map((new_de) => new_de.i);
      try {
        const response = await EMSESP.writeExcludeEntities({
          id: selectedDevice,
          entity_ids: exclude_entities
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

    const toggleDeviceEntity = (id: number) => {
      setDeviceEntities(
        deviceEntities.map((o) => {
          if (o.i === id) {
            return { ...o, x: !o.x };
          }
          return o;
        })
      );
    };

    return (
      <>
        <Table size="small">
          <TableHead>
            <TableRow>
              <StyledTableCell>
                ({deviceEntities.reduce((a, v) => (v.x ? a + 1 : a), 0)}/{deviceEntities.length})
              </StyledTableCell>
              <StyledTableCell align="left">Entity Name</StyledTableCell>
              <StyledTableCell>Code</StyledTableCell>
              <StyledTableCell align="right">Value</StyledTableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            {deviceEntities.map((de) => (
              <TableRow
                key={de.i}
                onClick={() => toggleDeviceEntity(de.i)}
                sx={de.x ? { backgroundColor: '#f8696b' } : { backgroundColor: 'black' }}
              >
                <StyledTableCell padding="checkbox">{de.x && <CloseIcon fontSize="small" />}</StyledTableCell>
                <StyledTableCell component="th" scope="row">
                  {de.n}
                </StyledTableCell>
                <StyledTableCell>{de.s}</StyledTableCell>
                <StyledTableCell align="right">{formatValue(de.v)}</StyledTableCell>
              </TableRow>
            ))}
          </TableBody>
        </Table>
      </>
    );
  };

  const content = () => {
    return (
      <>
        {renderDeviceList()}
        {renderDeviceData()}
        <ButtonRow>
          <Button startIcon={<SaveIcon />} variant="outlined" color="primary" onClick={() => saveCustomization()}>
            Save
          </Button>
        </ButtonRow>
      </>
    );
  };

  return (
    <SectionContent title="Device Entity Customization" titleGutter>
      {content()}
    </SectionContent>
  );
};

export default SettingsCustomization;
