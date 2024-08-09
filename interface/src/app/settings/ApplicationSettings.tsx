import { useState } from 'react';
import { toast } from 'react-toastify';

import CancelIcon from '@mui/icons-material/Cancel';
import PowerSettingsNewIcon from '@mui/icons-material/PowerSettingsNew';
import WarningIcon from '@mui/icons-material/Warning';
import {
  Box,
  Button,
  Checkbox,
  Divider,
  Grid,
  InputAdornment,
  MenuItem,
  TextField,
  Typography
} from '@mui/material';

import { readHardwareStatus, restart } from 'api/system';

import { useRequest } from 'alova/client';
import RestartMonitor from 'app/status/RestartMonitor';
import type { ValidateFieldsError } from 'async-validator';
import {
  BlockFormControlLabel,
  BlockNavigation,
  ButtonRow,
  FormLoader,
  MessageBox,
  SectionContent,
  ValidatedTextField,
  useLayoutTitle
} from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import { numberValue, updateValueDirty, useRest } from 'utils';
import { validate } from 'validators';

import { getBoardProfile, readSettings, writeSettings } from '../../api/app';
import { BOARD_PROFILES } from '../main/types';
import type { Settings } from '../main/types';
import { createSettingsValidator } from '../main/validators';

export function boardProfileSelectItems() {
  return Object.keys(BOARD_PROFILES).map((code) => (
    <MenuItem key={code} value={code}>
      {BOARD_PROFILES[code]}
    </MenuItem>
  ));
}

const ApplicationSettings = () => {
  const { data: hardwareData } = useRequest(readHardwareStatus, {
    initialData: { psram: false }
  });

  const {
    loadData,
    saveData,
    saving,
    updateDataValue,
    data,
    origData,
    dirtyFlags,
    setDirtyFlags,
    blocker,
    errorMessage,
    restartNeeded
  } = useRest<Settings>({
    read: readSettings,
    update: writeSettings
  });

  const [restarting, setRestarting] = useState<boolean>();

  const { LL } = useI18nContext();

  const updateFormValue = updateValueDirty(
    origData,
    dirtyFlags,
    setDirtyFlags,
    updateDataValue
  );

  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();

  const { loading: processingBoard, send: readBoardProfile } = useRequest(
    (boardProfile: string) => getBoardProfile(boardProfile),
    {
      immediate: false
    }
  ).onSuccess((event) => {
    const response = event.data as Settings;
    updateDataValue({
      ...data,
      board_profile: response.board_profile,
      led_gpio: response.led_gpio,
      dallas_gpio: response.dallas_gpio,
      rx_gpio: response.rx_gpio,
      tx_gpio: response.tx_gpio,
      pbutton_gpio: response.pbutton_gpio,
      phy_type: response.phy_type,
      eth_power: response.eth_power,
      eth_phy_addr: response.eth_phy_addr,
      eth_clock_mode: response.eth_clock_mode
    });
  });

  const { send: restartCommand } = useRequest(restart(), {
    immediate: false
  });

  const updateBoardProfile = async (board_profile: string) => {
    await readBoardProfile(board_profile).catch((error: Error) => {
      toast.error(error.message);
    });
  };

  useLayoutTitle(LL.SETTINGS_OF(LL.APPLICATION()));

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage} />;
    }

    const validateAndSubmit = async () => {
      try {
        setFieldErrors(undefined);
        await validate(createSettingsValidator(data), data);
      } catch (error) {
        setFieldErrors(error as ValidateFieldsError);
      } finally {
        await saveData();
      }
    };

    const changeBoardProfile = (event: React.ChangeEvent<HTMLInputElement>) => {
      const boardProfile = event.target.value;
      updateFormValue(event);
      if (boardProfile === 'CUSTOM') {
        updateDataValue({
          ...data,
          board_profile: boardProfile
        });
      } else {
        void updateBoardProfile(boardProfile);
      }
    };

    const restart = async () => {
      await validateAndSubmit();
      await restartCommand().catch((error: Error) => {
        toast.error(error.message);
      });
      setRestarting(true);
    };

    return (
      <>
        <Typography sx={{ pb: 1 }} variant="h6" color="primary">
          {LL.INTERFACE_BOARD_PROFILE()}
        </Typography>
        <Box color="warning.main">
          <Typography variant="body2">{LL.BOARD_PROFILE_TEXT()}</Typography>
        </Box>
        <TextField
          name="board_profile"
          label={LL.BOARD_PROFILE()}
          value={data.board_profile}
          disabled={processingBoard || hardwareData.model.startsWith('BBQKees')}
          fullWidth
          variant="outlined"
          onChange={changeBoardProfile}
          margin="normal"
          select
        >
          {boardProfileSelectItems()}
          <Divider />
          <MenuItem key={'CUSTOM'} value={'CUSTOM'}>
            {LL.CUSTOM()}&hellip;
          </MenuItem>
        </TextField>
        {data.board_profile === 'CUSTOM' && (
          <>
            <Grid
              container
              spacing={1}
              sx={{ pt: 1 }}
              direction="row"
              justifyContent="flex-start"
              alignItems="flex-start"
            >
              <Grid item xs={12} sm={6} md={4}>
                <ValidatedTextField
                  fieldErrors={fieldErrors}
                  name="rx_gpio"
                  label={LL.GPIO_OF('Rx')}
                  fullWidth
                  variant="outlined"
                  value={numberValue(data.rx_gpio)}
                  type="number"
                  onChange={updateFormValue}
                  margin="normal"
                  disabled={saving}
                />
              </Grid>
              <Grid item xs={12} sm={6} md={4}>
                <ValidatedTextField
                  fieldErrors={fieldErrors}
                  name="tx_gpio"
                  label={LL.GPIO_OF('Tx')}
                  fullWidth
                  variant="outlined"
                  value={numberValue(data.tx_gpio)}
                  type="number"
                  onChange={updateFormValue}
                  margin="normal"
                  disabled={saving}
                />
              </Grid>
              <Grid item xs={12} sm={6} md={4}>
                <ValidatedTextField
                  fieldErrors={fieldErrors}
                  name="pbutton_gpio"
                  label={LL.GPIO_OF(LL.BUTTON())}
                  fullWidth
                  variant="outlined"
                  value={numberValue(data.pbutton_gpio)}
                  type="number"
                  onChange={updateFormValue}
                  margin="normal"
                  disabled={saving}
                />
              </Grid>
              <Grid item xs={12} sm={6} md={4}>
                <ValidatedTextField
                  fieldErrors={fieldErrors}
                  name="dallas_gpio"
                  label={
                    LL.GPIO_OF(LL.TEMPERATURE()) + ' (0=' + LL.DISABLED(1) + ')'
                  }
                  fullWidth
                  variant="outlined"
                  value={numberValue(data.dallas_gpio)}
                  type="number"
                  onChange={updateFormValue}
                  margin="normal"
                  disabled={saving}
                />
              </Grid>
              <Grid item xs={12} sm={6} md={4}>
                <ValidatedTextField
                  fieldErrors={fieldErrors}
                  name="led_gpio"
                  label={LL.GPIO_OF('LED') + ' (0=' + LL.DISABLED(1) + ')'}
                  fullWidth
                  variant="outlined"
                  value={numberValue(data.led_gpio)}
                  type="number"
                  onChange={updateFormValue}
                  margin="normal"
                  disabled={saving}
                />
              </Grid>
              <Grid item xs={12} sm={6} md={4}>
                <TextField
                  name="phy_type"
                  label={LL.PHY_TYPE()}
                  disabled={saving}
                  value={data.phy_type}
                  fullWidth
                  variant="outlined"
                  onChange={updateFormValue}
                  margin="normal"
                  select
                >
                  <MenuItem value={0}>{LL.DISABLED(1)}</MenuItem>
                  <MenuItem value={1}>LAN8720</MenuItem>
                  <MenuItem value={2}>TLK110</MenuItem>
                </TextField>
              </Grid>
            </Grid>
            {data.phy_type !== 0 && (
              <Grid
                container
                spacing={1}
                sx={{ pt: 1 }}
                direction="row"
                justifyContent="flex-start"
                alignItems="flex-start"
              >
                <Grid item xs={12} sm={6} md={4}>
                  <TextField
                    name="eth_power"
                    label={LL.GPIO_OF('PHY Power') + ' (-1=' + LL.DISABLED(1) + ')'}
                    fullWidth
                    variant="outlined"
                    value={numberValue(data.eth_power)}
                    type="number"
                    onChange={updateFormValue}
                    margin="normal"
                    disabled={saving}
                  />
                </Grid>
                <Grid item xs={12} sm={6} md={4}>
                  <TextField
                    name="eth_phy_addr"
                    label={LL.ADDRESS_OF('PHY I²C')}
                    fullWidth
                    variant="outlined"
                    value={numberValue(data.eth_phy_addr)}
                    type="number"
                    onChange={updateFormValue}
                    margin="normal"
                    disabled={saving}
                  />
                </Grid>
                <Grid item xs={12} sm={6} md={4}>
                  <TextField
                    name="eth_clock_mode"
                    label="PHY Clk"
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
                  </TextField>
                </Grid>
              </Grid>
            )}
          </>
        )}
        <Typography sx={{ pt: 2 }} variant="h6" color="primary">
          {LL.SETTINGS_OF(LL.EMS_BUS(0))}
        </Typography>
        <Grid
          container
          spacing={1}
          direction="row"
          justifyContent="flex-start"
          alignItems="flex-start"
        >
          <Grid item xs={12} sm={6}>
            <TextField
              name="tx_mode"
              label={LL.TX_MODE()}
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
              <MenuItem value={4}>{LL.HARDWARE()}</MenuItem>
            </TextField>
          </Grid>
          <Grid item xs={12} sm={6}>
            <TextField
              name="ems_bus_id"
              label={LL.ID_OF(LL.EMS_BUS(1))}
              disabled={saving}
              value={data.ems_bus_id}
              fullWidth
              variant="outlined"
              onChange={updateFormValue}
              margin="normal"
              select
            >
              <MenuItem value={0x0a}>Terminal (0x0A)</MenuItem>
              <MenuItem value={0x0b}>Service Key (0x0B)</MenuItem>
              <MenuItem value={0x0d}>Modem (0x0D)</MenuItem>
              <MenuItem value={0x0e}>Converter (0x0E)</MenuItem>
              <MenuItem value={0x0f}>Time Module (0x0F)</MenuItem>
              <MenuItem value={0x48}>Gateway 1 (0x48)</MenuItem>
              <MenuItem value={0x49}>Gateway 2 (0x49)</MenuItem>
              <MenuItem value={0x4a}>Gateway 3 (0x4A)</MenuItem>
              <MenuItem value={0x4b}>Gateway 4 (0x4B)</MenuItem>
              <MenuItem value={0x4c}>Gateway 5 (0x4C)</MenuItem>
              <MenuItem value={0x4d}>Gateway 7 (0x4D)</MenuItem>
            </TextField>
          </Grid>
        </Grid>
        <Typography sx={{ pt: 2 }} variant="h6" color="primary">
          {LL.GENERAL_OPTIONS()}
        </Typography>
        <Grid item>
          <TextField
            name="locale"
            label={LL.LANGUAGE_ENTITIES()}
            disabled={saving}
            value={data.locale}
            fullWidth
            variant="outlined"
            onChange={updateFormValue}
            margin="normal"
            select
          >
            <MenuItem value="de">Deutsch (DE)</MenuItem>
            <MenuItem value="en">English (EN)</MenuItem>
            <MenuItem value="fr">Français (FR)</MenuItem>
            <MenuItem value="it">Italiano (IT)</MenuItem>
            <MenuItem value="nl">Nederlands (NL)</MenuItem>
            <MenuItem value="no">Norsk (NO)</MenuItem>
            <MenuItem value="pl">Polski (PL)</MenuItem>
            <MenuItem value="sk">Slovenčina (SK)</MenuItem>
            <MenuItem value="sv">Svenska (SV)</MenuItem>
            <MenuItem value="tr">Türk (TR)</MenuItem>
          </TextField>
        </Grid>
        {data.led_gpio !== 0 && (
          <BlockFormControlLabel
            control={
              <Checkbox
                checked={data.hide_led}
                onChange={updateFormValue}
                name="hide_led"
              />
            }
            label={LL.HIDE_LED()}
            disabled={saving}
          />
        )}
        <BlockFormControlLabel
          control={
            <Checkbox
              checked={data.telnet_enabled}
              onChange={updateFormValue}
              name="telnet_enabled"
            />
          }
          label={LL.ENABLE_TELNET()}
          disabled={saving}
        />
        <BlockFormControlLabel
          control={
            <Checkbox
              checked={data.analog_enabled}
              onChange={updateFormValue}
              name="analog_enabled"
            />
          }
          label={LL.ENABLE_ANALOG()}
          disabled={saving}
        />
        <BlockFormControlLabel
          control={
            <Checkbox
              checked={data.fahrenheit}
              onChange={updateFormValue}
              name="fahrenheit"
            />
          }
          label={LL.CONVERT_FAHRENHEIT()}
          disabled={saving}
        />
        <BlockFormControlLabel
          control={
            <Checkbox
              checked={data.notoken_api}
              onChange={updateFormValue}
              name="notoken_api"
            />
          }
          label={LL.BYPASS_TOKEN()}
          disabled={saving}
        />
        <BlockFormControlLabel
          control={
            <Checkbox
              checked={data.readonly_mode}
              onChange={updateFormValue}
              name="readonly_mode"
            />
          }
          label={LL.READONLY()}
          disabled={saving}
        />
        <BlockFormControlLabel
          control={
            <Checkbox
              checked={data.low_clock}
              onChange={updateFormValue}
              name="low_clock"
            />
          }
          label={LL.UNDERCLOCK_CPU()}
          disabled={saving}
        />
        <BlockFormControlLabel
          control={
            <Checkbox
              checked={data.boiler_heatingoff}
              onChange={updateFormValue}
              name="boiler_heatingoff"
            />
          }
          label={LL.HEATINGOFF()}
          disabled={saving}
        />
        <Grid
          container
          spacing={1}
          direction="row"
          justifyContent="flex-start"
          alignItems="flex-start"
        >
          <Grid item xs={12} sm={6} md={4}>
            <BlockFormControlLabel
              control={
                <Checkbox
                  checked={data.remote_timeout_en}
                  onChange={updateFormValue}
                  name="remote_timeout_en"
                />
              }
              label={LL.REMOTE_TIMEOUT_EN()}
            />
          </Grid>
          {data.remote_timeout_en && (
            <Grid item xs={12} sm={6} md={4}>
              <ValidatedTextField
                fieldErrors={fieldErrors}
                name="remote_timeout"
                label={LL.REMOTE_TIMEOUT()}
                InputProps={{
                  endAdornment: (
                    <InputAdornment position="end">{LL.HOURS()}</InputAdornment>
                  )
                }}
                fullWidth
                variant="outlined"
                value={numberValue(data.remote_timeout)}
                type="number"
                onChange={updateFormValue}
              />
            </Grid>
          )}
        </Grid>
        <Grid
          container
          spacing={0}
          direction="row"
          justifyContent="flex-start"
          alignItems="flex-start"
        >
          <BlockFormControlLabel
            control={
              <Checkbox
                checked={data.shower_timer}
                onChange={updateFormValue}
                name="shower_timer"
              />
            }
            label={LL.ENABLE_SHOWER_TIMER()}
            disabled={saving}
          />
          <BlockFormControlLabel
            control={
              <Checkbox
                checked={data.shower_alert}
                onChange={updateFormValue}
                name="shower_alert"
              />
            }
            label={LL.ENABLE_SHOWER_ALERT()}
            disabled={!data.shower_timer}
          />
        </Grid>
        <Grid
          container
          sx={{ pt: 2 }}
          rowSpacing={3}
          spacing={1}
          direction="row"
          justifyContent="flex-start"
          alignItems="flex-start"
        >
          {data.shower_timer && (
            <Grid item xs={12} sm={6}>
              <ValidatedTextField
                fieldErrors={fieldErrors}
                name="shower_min_duration"
                label={LL.MIN_DURATION()}
                InputProps={{
                  endAdornment: (
                    <InputAdornment position="end">{LL.SECONDS()}</InputAdornment>
                  )
                }}
                variant="outlined"
                value={numberValue(data.shower_min_duration)}
                fullWidth
                type="number"
                onChange={updateFormValue}
              />
            </Grid>
          )}
          {data.shower_alert && (
            <>
              <Grid item xs={12} sm={6}>
                <ValidatedTextField
                  fieldErrors={fieldErrors}
                  name="shower_alert_trigger"
                  label={LL.TRIGGER_TIME()}
                  InputProps={{
                    endAdornment: (
                      <InputAdornment position="end">{LL.MINUTES()}</InputAdornment>
                    )
                  }}
                  variant="outlined"
                  value={numberValue(data.shower_alert_trigger)}
                  fullWidth
                  type="number"
                  onChange={updateFormValue}
                  disabled={!data.shower_timer}
                />
              </Grid>
              <Grid item xs={12} sm={6}>
                <ValidatedTextField
                  fieldErrors={fieldErrors}
                  name="shower_alert_coldshot"
                  label={LL.COLD_SHOT_DURATION()}
                  InputProps={{
                    endAdornment: (
                      <InputAdornment position="end">{LL.SECONDS()}</InputAdornment>
                    )
                  }}
                  variant="outlined"
                  value={numberValue(data.shower_alert_coldshot)}
                  fullWidth
                  type="number"
                  onChange={updateFormValue}
                  disabled={!data.shower_timer}
                />
              </Grid>
            </>
          )}
        </Grid>
        <Typography sx={{ pt: 3 }} variant="h6" color="primary">
          {LL.FORMATTING_OPTIONS()}
        </Typography>
        <Grid
          container
          spacing={1}
          direction="row"
          justifyContent="flex-start"
          alignItems="flex-start"
        >
          <Grid item xs={12} sm={6} md={4}>
            <TextField
              name="bool_dashboard"
              label={LL.BOOLEAN_FORMAT_DASHBOARD()}
              value={data.bool_dashboard}
              fullWidth
              variant="outlined"
              onChange={updateFormValue}
              margin="normal"
              select
            >
              <MenuItem value={1}>{LL.ONOFF()}</MenuItem>
              <MenuItem value={2}>{LL.ONOFF_CAP()}</MenuItem>
              <MenuItem value={3}>true/false</MenuItem>
              <MenuItem value={5}>1/0</MenuItem>
            </TextField>
          </Grid>
          <Grid item xs={12} sm={6} md={4}>
            <TextField
              name="bool_format"
              label={LL.BOOLEAN_FORMAT_API()}
              value={data.bool_format}
              fullWidth
              variant="outlined"
              onChange={updateFormValue}
              margin="normal"
              select
            >
              <MenuItem value={1}>{LL.ONOFF()}</MenuItem>
              <MenuItem value={2}>{LL.ONOFF_CAP()}</MenuItem>
              <MenuItem value={3}>&quot;true&quot;/&quot;false&quot;</MenuItem>
              <MenuItem value={4}>true/false</MenuItem>
              <MenuItem value={5}>&quot;1&quot;/&quot;0&quot;</MenuItem>
              <MenuItem value={6}>1/0</MenuItem>
            </TextField>
          </Grid>
          <Grid item xs={12} sm={6} md={4}>
            <TextField
              name="enum_format"
              label={LL.ENUM_FORMAT()}
              value={data.enum_format}
              fullWidth
              variant="outlined"
              onChange={updateFormValue}
              margin="normal"
              select
            >
              <MenuItem value={1}>{LL.VALUE(5)}</MenuItem>
              <MenuItem value={2}>{LL.INDEX()}</MenuItem>
            </TextField>
          </Grid>
        </Grid>
        {data.dallas_gpio !== 0 && (
          <>
            <Typography sx={{ pt: 2 }} variant="h6" color="primary">
              {LL.TEMP_SENSORS()}
            </Typography>
            <BlockFormControlLabel
              control={
                <Checkbox
                  checked={data.dallas_parasite}
                  onChange={updateFormValue}
                  name="dallas_parasite"
                />
              }
              label={LL.ENABLE_PARASITE()}
              disabled={saving}
            />
          </>
        )}
        <Typography sx={{ pt: 2 }} variant="h6" color="primary">
          {LL.LOGGING()}
        </Typography>
        <BlockFormControlLabel
          control={
            <Checkbox
              checked={data.trace_raw}
              onChange={updateFormValue}
              name="trace_raw"
            />
          }
          label={LL.LOG_HEX()}
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
          label={LL.ENABLE_SYSLOG()}
        />
        {data.syslog_enabled && (
          <Grid
            container
            spacing={1}
            direction="row"
            justifyContent="flex-start"
            alignItems="flex-start"
          >
            <Grid item xs={12} sm={6}>
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
            <Grid item xs={12} sm={6}>
              <ValidatedTextField
                fieldErrors={fieldErrors}
                name="syslog_port"
                label="Port"
                fullWidth
                variant="outlined"
                value={numberValue(data.syslog_port)}
                type="number"
                onChange={updateFormValue}
                margin="normal"
                disabled={saving}
              />
            </Grid>
            <Grid item xs={12} sm={6}>
              <TextField
                name="syslog_level"
                label={LL.LOG_LEVEL()}
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
              </TextField>
            </Grid>
            <Grid item xs={12} sm={6}>
              <ValidatedTextField
                fieldErrors={fieldErrors}
                name="syslog_mark_interval"
                label={LL.MARK_INTERVAL()}
                InputProps={{
                  endAdornment: (
                    <InputAdornment position="end">{LL.SECONDS()}</InputAdornment>
                  )
                }}
                fullWidth
                variant="outlined"
                value={numberValue(data.syslog_mark_interval)}
                type="number"
                onChange={updateFormValue}
                margin="normal"
                disabled={saving}
              />
            </Grid>
          </Grid>
        )}
        <Typography sx={{ pt: 2 }} variant="h6" color="primary">
          Modbus
        </Typography>
        <BlockFormControlLabel
          control={
            <Checkbox
              checked={data.modbus_enabled}
              onChange={updateFormValue}
              name="modbus_enabled"
              disabled={!hardwareData.psram}
            />
          }
          label={LL.ENABLE_MODBUS()}
        />
        {data.modbus_enabled && (
          <Grid
            container
            spacing={1}
            direction="row"
            justifyContent="flex-start"
            alignItems="flex-start"
          >
            <Grid item xs={12} sm={6}>
              <ValidatedTextField
                fieldErrors={fieldErrors}
                name="modbus_max_clients"
                label={LL.AP_MAX_CLIENTS()}
                fullWidth
                variant="outlined"
                value={numberValue(data.modbus_max_clients)}
                type="number"
                onChange={updateFormValue}
                margin="normal"
                disabled={saving}
              />
            </Grid>
            <Grid item xs={12} sm={6}>
              <ValidatedTextField
                fieldErrors={fieldErrors}
                name="modbus_port"
                label="Port"
                fullWidth
                variant="outlined"
                value={numberValue(data.modbus_port)}
                type="number"
                onChange={updateFormValue}
                margin="normal"
                disabled={saving}
              />
            </Grid>
            <Grid item xs={12} sm={6}>
              <ValidatedTextField
                fieldErrors={fieldErrors}
                name="modbus_timeout"
                label="Timeout"
                InputProps={{
                  endAdornment: <InputAdornment position="end">ms</InputAdornment>
                }}
                fullWidth
                variant="outlined"
                value={numberValue(data.modbus_timeout)}
                type="number"
                onChange={updateFormValue}
                margin="normal"
                disabled={saving}
              />
            </Grid>
          </Grid>
        )}
        {restartNeeded && (
          <MessageBox my={2} level="warning" message={LL.RESTART_TEXT(0)}>
            <Button
              startIcon={<PowerSettingsNewIcon />}
              variant="contained"
              color="error"
              onClick={restart}
            >
              {LL.RESTART()}
            </Button>
          </MessageBox>
        )}
        {!restartNeeded && dirtyFlags && dirtyFlags.length !== 0 && (
          <ButtonRow>
            <Button
              startIcon={<CancelIcon />}
              disabled={saving}
              variant="outlined"
              color="primary"
              type="submit"
              onClick={loadData}
            >
              {LL.CANCEL()}
            </Button>
            <Button
              startIcon={<WarningIcon color="warning" />}
              disabled={saving}
              variant="contained"
              color="info"
              type="submit"
              onClick={validateAndSubmit}
            >
              {LL.APPLY_CHANGES(dirtyFlags.length)}
            </Button>
          </ButtonRow>
        )}
      </>
    );
  };

  return (
    <SectionContent>
      {blocker ? <BlockNavigation blocker={blocker} /> : null}
      {restarting ? <RestartMonitor /> : content()}
    </SectionContent>
  );
};

export default ApplicationSettings;
