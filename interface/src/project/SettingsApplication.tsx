import { FC, useState } from 'react';
import { ValidateFieldsError } from 'async-validator';

import { useSnackbar } from 'notistack';

import { Box, Button, Checkbox, MenuItem, Grid, Typography, Divider, InputAdornment } from '@mui/material';

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
    update: EMSESP.writeSettings
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
          phy_type: response.data.phy_type,
          eth_power: response.data.eth_power,
          eth_phy_addr: response.data.eth_phy_addr,
          eth_clock_mode: response.data.eth_clock_mode
        });
      }
    } catch (error: unknown) {
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
        enqueueSnackbar('EMS-ESP is restarting...', { variant: 'info' });
      } catch (error: unknown) {
        enqueueSnackbar(extractErrorMessage(error, 'Problem restarting device'), { variant: 'error' });
      }
    };

    return (
      <>
        <Typography sx={{ pt: 2 }} variant="h6" color="primary">
          Interface Board Profile
        </Typography>
        <Box color="warning.main">
          <Typography variant="body2">
            Select a pre-configured interface board profile from the list below or choose "Custom" to configure your own
            hardware settings
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
          <Divider />
          <MenuItem key={'CUSTOM'} value={'CUSTOM'}>
            Custom&hellip;
          </MenuItem>
        </ValidatedTextField>
        {data.board_profile === 'CUSTOM' && (
          <>
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
              <Grid item>
                <ValidatedTextField
                  fieldErrors={fieldErrors}
                  name="dallas_gpio"
                  label="Temperature GPIO (0=disabled)"
                  fullWidth
                  variant="outlined"
                  value={numberValue(data.dallas_gpio)}
                  type="number"
                  onChange={updateFormValue}
                  margin="normal"
                  disabled={saving}
                />
              </Grid>
              <Grid item>
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
            </Grid>
            <Grid item xs={4}>
              <ValidatedTextField
                name="phy_type"
                label="Eth PHY Type"
                disabled={saving}
                value={data.phy_type}
                fullWidth
                variant="outlined"
                onChange={updateFormValue}
                margin="normal"
                select
              >
                <MenuItem value={0}>No Ethernet Module</MenuItem>
                <MenuItem value={1}>LAN8720</MenuItem>
                <MenuItem value={2}>TLK110</MenuItem>
              </ValidatedTextField>
            </Grid>
            {data.phy_type !== 0 && (
              <Grid container spacing={1} direction="row" justifyContent="flex-start" alignItems="flex-start">
                <Grid item>
                  <ValidatedTextField
                    name="eth_power"
                    label="Eth Power GPIO (-1=disabled)"
                    fullWidth
                    variant="outlined"
                    value={numberValue(data.eth_power)}
                    type="number"
                    onChange={updateFormValue}
                    margin="normal"
                    disabled={saving}
                  />
                </Grid>
                <Grid item>
                  <ValidatedTextField
                    name="eth_phy_addr"
                    label="Eth I²C-address"
                    fullWidth
                    variant="outlined"
                    value={numberValue(data.eth_phy_addr)}
                    type="number"
                    onChange={updateFormValue}
                    margin="normal"
                    disabled={saving}
                  />
                </Grid>
                <Grid item>
                  <ValidatedTextField
                    name="eth_clock_mode"
                    label="Eth Clock Mode"
                    disabled={saving}
                    value={data.eth_clock_mode}
                    fullWidth
                    variant="outlined"
                    onChange={updateFormValue}
                    margin="normal"
                    select
                  >
                    <MenuItem value={0}>GPIO0_IN</MenuItem>
                    <MenuItem value={1}>GPIO0_OUT</MenuItem>
                    <MenuItem value={2}>GPIO16_OUT</MenuItem>
                    <MenuItem value={3}>GPIO17_OUT</MenuItem>
                  </ValidatedTextField>
                </Grid>
              </Grid>
            )}
          </>
        )}
        <Typography variant="h6" color="primary">
          EMS Bus Settings
        </Typography>
        <Grid container spacing={1} direction="row" justifyContent="flex-start" alignItems="flex-start">
          <Grid item xs={6}>
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
              <MenuItem value={1}>EMS</MenuItem>
              <MenuItem value={2}>EMS+</MenuItem>
              <MenuItem value={3}>HT3</MenuItem>
              <MenuItem value={4}>Hardware</MenuItem>
            </ValidatedTextField>
          </Grid>
          <Grid item xs={6}>
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
        <Box
          sx={{
            '& .MuiTextField-root': { width: '25ch' }
          }}
        >
          <ValidatedTextField
            name="locale"
            label="Language (for device entities)"
            disabled={saving}
            value={data.locale}
            variant="outlined"
            onChange={updateFormValue}
            margin="normal"
            size="small"
            select
          >
            <MenuItem value="en">English</MenuItem>
            <MenuItem value="de">Deutsch</MenuItem>
          </ValidatedTextField>
        </Box>
        {data.led_gpio !== 0 && (
          <BlockFormControlLabel
            control={<Checkbox checked={data.hide_led} onChange={updateFormValue} name="hide_led" />}
            label="Hide LED"
            disabled={saving}
          />
        )}
        <BlockFormControlLabel
          control={<Checkbox checked={data.telnet_enabled} onChange={updateFormValue} name="telnet_enabled" />}
          label="Enable Telnet Console"
          disabled={saving}
        />
        <BlockFormControlLabel
          control={<Checkbox checked={data.analog_enabled} onChange={updateFormValue} name="analog_enabled" />}
          label="Enable Analog Sensors"
          disabled={saving}
        />
        <BlockFormControlLabel
          control={<Checkbox checked={data.fahrenheit} onChange={updateFormValue} name="fahrenheit" />}
          label="Convert temperature values to Fahrenheit"
          disabled={saving}
        />
        <BlockFormControlLabel
          control={<Checkbox checked={data.notoken_api} onChange={updateFormValue} name="notoken_api" />}
          label="Bypass Access Token authorization on API calls"
          disabled={saving}
        />
        <BlockFormControlLabel
          control={<Checkbox checked={data.readonly_mode} onChange={updateFormValue} name="readonly_mode" />}
          label="Enable read-only mode (blocks all outgoing EMS Tx Write commands)"
          disabled={saving}
        />
        <BlockFormControlLabel
          control={<Checkbox checked={data.low_clock} onChange={updateFormValue} name="low_clock" />}
          label="Underclock CPU speed"
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
            disabled={!data.shower_timer}
          />
          {data.shower_alert && (
            <>
              <Grid item xs={4}>
                <ValidatedTextField
                  fieldErrors={fieldErrors}
                  name="shower_alert_trigger"
                  label="Trigger Time"
                  InputProps={{
                    endAdornment: <InputAdornment position="end">minutes</InputAdornment>
                  }}
                  variant="outlined"
                  value={data.shower_alert_trigger}
                  type="number"
                  onChange={updateFormValue}
                  disabled={!data.shower_timer}
                />
              </Grid>
              <Grid item xs={4}>
                <ValidatedTextField
                  fieldErrors={fieldErrors}
                  name="shower_alert_coldshot"
                  label="Cold Shot Duration"
                  InputProps={{
                    endAdornment: <InputAdornment position="end">seconds</InputAdornment>
                  }}
                  variant="outlined"
                  value={data.shower_alert_coldshot}
                  type="number"
                  onChange={updateFormValue}
                  disabled={!data.shower_timer}
                />
              </Grid>
            </>
          )}
        </Grid>
        <Typography sx={{ pt: 2 }} variant="h6" color="primary">
          Formatting Options
        </Typography>
        <Grid container spacing={1} direction="row" justifyContent="flex-start" alignItems="flex-start">
          <Grid item xs={4}>
            <ValidatedTextField
              name="bool_dashboard"
              label="Boolean Format Dashboard"
              value={data.bool_dashboard}
              fullWidth
              variant="outlined"
              onChange={updateFormValue}
              margin="normal"
              select
            >
              <MenuItem value={1}>on/off</MenuItem>
              <MenuItem value={2}>ON/OFF</MenuItem>
              <MenuItem value={3}>true/false</MenuItem>
              <MenuItem value={5}>1/0</MenuItem>
            </ValidatedTextField>
          </Grid>
          <Grid item xs={4}>
            <ValidatedTextField
              name="bool_format"
              label="Boolean Format API/MQTT"
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
              label="Enum Format API/MQTT"
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
        </Grid>
        {data.dallas_gpio !== 0 && (
          <>
            <Typography sx={{ pt: 2 }} variant="h6" color="primary">
              Temperature Sensors
            </Typography>
            <BlockFormControlLabel
              control={<Checkbox checked={data.dallas_parasite} onChange={updateFormValue} name="dallas_parasite" />}
              label="Enable parasite power"
              disabled={saving}
            />
          </>
        )}
        <Typography sx={{ pt: 2 }} variant="h6" color="primary">
          Logging
        </Typography>
        <BlockFormControlLabel
          control={<Checkbox checked={data.trace_raw} onChange={updateFormValue} name="trace_raw" />}
          label="Log EMS telegrams in hexadecimal"
          disabled={saving}
        />
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
            <Grid item xs={4}>
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
            <Grid item xs={4}>
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
            <Grid item xs={4}>
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
                <MenuItem value={9}>ALL</MenuItem>
              </ValidatedTextField>
            </Grid>
            <Grid item xs={4}>
              <ValidatedTextField
                fieldErrors={fieldErrors}
                name="syslog_mark_interval"
                label="Mark Interval"
                InputProps={{
                  endAdornment: <InputAdornment position="end">seconds</InputAdornment>
                }}
                fullWidth
                variant="outlined"
                value={data.syslog_mark_interval}
                type="number"
                onChange={updateFormValue}
                margin="normal"
                disabled={saving}
              />
            </Grid>
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
    <SectionContent title="Application Settings" titleGutter>
      {content()}
    </SectionContent>
  );
};

export default SettingsApplication;
