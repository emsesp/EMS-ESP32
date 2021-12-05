import { FC, useState } from 'react';
import { ValidateFieldsError } from 'async-validator';

import { useSnackbar } from 'notistack';

import { Box, Button, Checkbox, MenuItem, Grid, Typography } from '@mui/material';

import SaveIcon from '@mui/icons-material/Save';
import PowerSettingsNewIcon from '@mui/icons-material/PowerSettingsNew';

import { validate } from '../validators';
import { createSettingsValidator } from './validators';

import {
  SectionContent,
  FormLoader,
  BlockFormControlLabel,
  ValidatedTextField,
  ButtonRow,
  MessageBox
} from '../components';
import { numberValue, extractErrorMessage, updateValue, useRest } from '../utils';

import * as EMSESP from './api';
import { Settings, BOARD_PROFILES } from './types';

export function boardProfileSelectItems() {
  return Object.keys(BOARD_PROFILES).map((code) => (
    <MenuItem key={code} value={code}>
      {BOARD_PROFILES[code]}
    </MenuItem>
  ));
}

const SettingsApplication: FC = () => {
  const { loadData, saveData, saving, setData, data, errorMessage, restartNeeded } = useRest<Settings>({
    read: EMSESP.readSettings,
    update: EMSESP.updateSettings
  });

  const { enqueueSnackbar } = useSnackbar();

  const updateFormValue = updateValue(setData);

  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();
  const [processingBoard, setProcessingBoard] = useState<boolean>(false);

  const updateBoardProfile = async (board_profile: string) => {
    setProcessingBoard(true);
    try {
      const response = await EMSESP.getBoardProfile({ board_profile: board_profile });
      if (data) {
        setData({
          ...data,
          board_profile: board_profile,
          led_gpio: response.data.led_gpio,
          dallas_gpio: response.data.dallas_gpio,
          rx_gpio: response.data.rx_gpio,
          tx_gpio: response.data.tx_gpio,
          pbutton_gpio: response.data.pbutton_gpio,
          phy_type: response.data.phy_type
        });
      }
    } catch (error: any) {
      enqueueSnackbar(extractErrorMessage(error, 'Problem fetching board profile'), { variant: 'error' });
    } finally {
      setProcessingBoard(false);
    }
  };

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage} />;
    }

    const validateAndSubmit = async () => {
      try {
        setFieldErrors(undefined);
        await validate(createSettingsValidator(data), data);
        saveData();
      } catch (errors: any) {
        setFieldErrors(errors);
      }
    };

    const changeBoardProfile = (event: React.ChangeEvent<HTMLInputElement>) => {
      const board_profile = event.target.value;
      if (board_profile === 'CUSTOM') {
        setData({
          ...data,
          board_profile: board_profile
        });
      } else {
        updateBoardProfile(board_profile);
      }
    };

    const restart = async () => {
      validateAndSubmit();
      try {
        await EMSESP.restart();
        enqueueSnackbar('Device is restarting', { variant: 'info' });
      } catch (error: any) {
        enqueueSnackbar(extractErrorMessage(error, 'Problem restarting device'), { variant: 'error' });
      }
    };

    return (
      <>
        <Typography sx={{ pt: 2 }} variant="h6" color="primary">
          Board Profile
        </Typography>
        <Box color="warning.main" p={0} mt={0} mb={0}>
          <Typography variant="body2">
            <i>Select a pre-configured board profile or use "Custom" to customize your own hardware settings</i>
          </Typography>
        </Box>
        <ValidatedTextField
          name="board_profile"
          label="Board Profile"
          value={data.board_profile}
          disabled={processingBoard}
          variant="outlined"
          onChange={changeBoardProfile}
          margin="normal"
          select
        >
          {boardProfileSelectItems()}
          <MenuItem key={'CUSTOM'} value={'CUSTOM'}>
            Custom...
          </MenuItem>
        </ValidatedTextField>
        {data.board_profile === 'CUSTOM' && (
          <Grid container spacing={1} direction="row" justifyContent="flex-start" alignItems="flex-start">
            <Grid item xs={4}>
              <ValidatedTextField
                fieldErrors={fieldErrors}
                name="rx_gpio"
                label="Rx GPIO"
                fullWidth
                variant="outlined"
                value={numberValue(data.rx_gpio)}
                type="number"
                onChange={updateFormValue}
                margin="normal"
                disabled={saving}
              />
            </Grid>
            <Grid item xs={4}>
              <ValidatedTextField
                fieldErrors={fieldErrors}
                name="tx_gpio"
                label="Tx GPIO"
                fullWidth
                variant="outlined"
                value={numberValue(data.tx_gpio)}
                type="number"
                onChange={updateFormValue}
                margin="normal"
                disabled={saving}
              />
            </Grid>
            <Grid item xs={4}>
              <ValidatedTextField
                fieldErrors={fieldErrors}
                name="pbutton_gpio"
                label="Button GPIO"
                fullWidth
                variant="outlined"
                value={numberValue(data.pbutton_gpio)}
                type="number"
                onChange={updateFormValue}
                margin="normal"
                disabled={saving}
              />
            </Grid>
            <Grid item xs={4}>
              <ValidatedTextField
                fieldErrors={fieldErrors}
                name="dallas_gpio"
                label="Dallas GPIO (0=disabled)"
                fullWidth
                variant="outlined"
                value={numberValue(data.dallas_gpio)}
                type="number"
                onChange={updateFormValue}
                margin="normal"
                disabled={saving}
              />
            </Grid>
            <Grid item xs={4}>
              <ValidatedTextField
                fieldErrors={fieldErrors}
                name="led_gpio"
                label="LED GPIO (0=disabled)"
                fullWidth
                variant="outlined"
                value={numberValue(data.led_gpio)}
                type="number"
                onChange={updateFormValue}
                margin="normal"
                disabled={saving}
              />
            </Grid>
            <Grid item xs={4}>
              <ValidatedTextField
                name="phy_type"
                label="PHY Module Type"
                disabled={saving}
                value={data.phy_type}
                fullWidth
                variant="outlined"
                onChange={updateFormValue}
                margin="normal"
                select
              >
                <MenuItem value={0}>No Ethernet</MenuItem>
                <MenuItem value={1}>LAN8720</MenuItem>
                <MenuItem value={2}>TLK110</MenuItem>
              </ValidatedTextField>
            </Grid>
          </Grid>
        )}
        <Typography variant="h6" color="primary">
          EMS Bus
        </Typography>
        <Grid container spacing={1} direction="row" justifyContent="flex-start" alignItems="flex-start">
          <Grid item xs={4}>
            <ValidatedTextField
              name="tx_mode"
              label="Tx Mode"
              disabled={saving}
              value={data.tx_mode}
              fullWidth
              variant="outlined"
              onChange={updateFormValue}
              margin="normal"
              select
            >
              <MenuItem value={0}>Off</MenuItem>
              <MenuItem value={1}>EMS</MenuItem>
              <MenuItem value={2}>EMS+</MenuItem>
              <MenuItem value={3}>HT3</MenuItem>
              <MenuItem value={4}>Hardware</MenuItem>
            </ValidatedTextField>
          </Grid>
          <Grid item>
            <ValidatedTextField
              name="ems_bus_id"
              label="Bus ID"
              disabled={saving}
              value={data.ems_bus_id}
              fullWidth
              variant="outlined"
              onChange={updateFormValue}
              margin="normal"
              select
            >
              <MenuItem value={0x0b}>Service Key (0x0B)</MenuItem>
              <MenuItem value={0x0d}>Modem (0x0D)</MenuItem>
              <MenuItem value={0x0a}>Terminal (0x0A)</MenuItem>
              <MenuItem value={0x0f}>Time Module (0x0F)</MenuItem>
              <MenuItem value={0x12}>Alarm Module (0x12)</MenuItem>
            </ValidatedTextField>
          </Grid>
        </Grid>
        <Typography sx={{ pt: 2 }} variant="h6" color="primary">
          General Options
        </Typography>
        {data.led_gpio !== 0 && (
          <BlockFormControlLabel
            control={<Checkbox checked={data.hide_led} onChange={updateFormValue} name="hide_led" />}
            label="Hide LED"
            disabled={saving}
          />
        )}
        <BlockFormControlLabel
          control={<Checkbox checked={data.disable_telnet} onChange={updateFormValue} name="disable_telnet" />}
          label="Disable Telnet Console"
          disabled={saving}
        />
        {data.dallas_gpio !== 0 && (
          <BlockFormControlLabel
            control={<Checkbox checked={data.dallas_parasite} onChange={updateFormValue} name="dallas_parasite" />}
            label="Use Dallas Sensor parasite power"
            disabled={saving}
          />
        )}
        <BlockFormControlLabel
          control={<Checkbox checked={data.analog_enabled} onChange={updateFormValue} name="analog_enabled" />}
          label="Enable ADC"
          disabled={saving}
        />
        <BlockFormControlLabel
          control={<Checkbox checked={data.low_clock} onChange={updateFormValue} name="low_clock" />}
          label="Run at a lower CPU clock speed"
          disabled={saving}
        />
        <BlockFormControlLabel
          control={<Checkbox checked={data.notoken_api} onChange={updateFormValue} name="notoken_api" />}
          label="Bypass Access Token authorization on API calls"
          disabled={saving}
        />
        <Grid container spacing={0} direction="row" justifyContent="flex-start" alignItems="flex-start">
          <BlockFormControlLabel
            control={<Checkbox checked={data.shower_timer} onChange={updateFormValue} name="shower_timer" />}
            label="Enable Shower Timer"
            disabled={saving}
          />
          <BlockFormControlLabel
            control={<Checkbox checked={data.shower_alert} onChange={updateFormValue} name="shower_alert" />}
            label="Enable Shower Alert"
            disabled={saving}
          />
        </Grid>
        <Typography sx={{ pt: 2 }} variant="h6" color="primary">
          Formatting Options
        </Typography>
        <Grid container spacing={1} direction="row" justifyContent="flex-start" alignItems="flex-start">
          <Grid item xs={4}>
            <ValidatedTextField
              name="bool_format"
              label="Boolean Format"
              value={data.bool_format}
              fullWidth
              variant="outlined"
              onChange={updateFormValue}
              margin="normal"
              select
            >
              <MenuItem value={1}>"on"/"off"</MenuItem>
              <MenuItem value={2}>"ON"/"OFF"</MenuItem>
              <MenuItem value={3}>"true"/"false"</MenuItem>
              <MenuItem value={4}>true/false</MenuItem>
              <MenuItem value={5}>"1"/"0"</MenuItem>
              <MenuItem value={6}>1/0</MenuItem>
            </ValidatedTextField>
          </Grid>
          <Grid item xs={4}>
            <ValidatedTextField
              name="enum_format"
              label="Enum Format"
              value={data.enum_format}
              fullWidth
              variant="outlined"
              onChange={updateFormValue}
              margin="normal"
              select
            >
              <MenuItem value={1}>Value</MenuItem>
              <MenuItem value={2}>Index</MenuItem>
            </ValidatedTextField>
          </Grid>
          <Grid item xs={4}>
            <ValidatedTextField
              name="dallas_format"
              label="Dallas Sensor Format"
              value={data.dallas_format}
              fullWidth
              variant="outlined"
              onChange={updateFormValue}
              margin="normal"
              select
            >
              <MenuItem value={1}>ID</MenuItem>
              <MenuItem value={2}>Sensor #</MenuItem>
              <MenuItem value={3}>Name</MenuItem>
            </ValidatedTextField>
          </Grid>
        </Grid>
        <Typography sx={{ pt: 2 }} variant="h6" color="primary">
          Syslog
        </Typography>
        <BlockFormControlLabel
          control={
            <Checkbox
              checked={data.syslog_enabled}
              onChange={updateFormValue}
              name="syslog_enabled"
              disabled={saving}
            />
          }
          label="Enable Syslog"
        />
        {data.syslog_enabled && (
          <Grid container spacing={1} direction="row" justifyContent="flex-start" alignItems="flex-start">
            <Grid item xs={5}>
              <ValidatedTextField
                fieldErrors={fieldErrors}
                name="syslog_host"
                label="Host"
                fullWidth
                variant="outlined"
                value={data.syslog_host}
                onChange={updateFormValue}
                margin="normal"
                disabled={saving}
              />
            </Grid>
            <Grid item xs={6}>
              <ValidatedTextField
                fieldErrors={fieldErrors}
                name="syslog_port"
                label="Port"
                fullWidth
                variant="outlined"
                value={data.syslog_port}
                type="number"
                onChange={updateFormValue}
                margin="normal"
                disabled={saving}
              />
            </Grid>
            <Grid item xs={5}>
              <ValidatedTextField
                name="syslog_level"
                label="Log Level"
                value={data.syslog_level}
                fullWidth
                variant="outlined"
                onChange={updateFormValue}
                margin="normal"
                select
                disabled={saving}
              >
                <MenuItem value={-1}>OFF</MenuItem>
                <MenuItem value={3}>ERR</MenuItem>
                <MenuItem value={5}>NOTICE</MenuItem>
                <MenuItem value={6}>INFO</MenuItem>
                <MenuItem value={7}>DEBUG</MenuItem>
                <MenuItem value={8}>ALL</MenuItem>
              </ValidatedTextField>
            </Grid>
            <Grid item xs={6}>
              <ValidatedTextField
                fieldErrors={fieldErrors}
                name="syslog_mark_interval"
                label="Mark Interval (seconds, 0=off)"
                fullWidth
                variant="outlined"
                value={data.syslog_mark_interval}
                type="number"
                onChange={updateFormValue}
                margin="normal"
                disabled={saving}
              />
            </Grid>
            <BlockFormControlLabel
              control={<Checkbox checked={data.trace_raw} onChange={updateFormValue} name="trace_raw" />}
              label="Output EMS telegrams as hexadecimal bytes"
              disabled={saving}
            />
          </Grid>
        )}
        {restartNeeded && (
          <MessageBox my={2} level="warning" message="EMS-ESP needs to be restarted to apply changed system settings">
            <Button startIcon={<PowerSettingsNewIcon />} variant="contained" color="error" onClick={restart}>
              Restart
            </Button>
          </MessageBox>
        )}
        {!restartNeeded && (
          <ButtonRow>
            <Button
              startIcon={<SaveIcon />}
              disabled={saving}
              variant="outlined"
              color="primary"
              type="submit"
              onClick={validateAndSubmit}
            >
              Save
            </Button>
          </ButtonRow>
        )}
      </>
    );
  };

  return (
    <SectionContent title="EMS-ESP Application Settings" titleGutter>
      {content()}
    </SectionContent>
  );
};

export default SettingsApplication;
