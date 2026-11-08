import { useEffect, useRef, useState } from 'react';

import AccessTimeIcon from '@mui/icons-material/AccessTime';
import CancelIcon from '@mui/icons-material/Cancel';
import WarningIcon from '@mui/icons-material/Warning';
import {
  Box,
  Button,
  Checkbox,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  Grid,
  MenuItem,
  TextField,
  Typography
} from '@mui/material';

import * as NTPApi from 'api/ntp';
import { readNTPSettings } from 'api/ntp';

import { dialogStyle } from 'CustomTheme';
import { useRequest } from 'alova/client';
import { updateState } from 'alova/client';
import {
  BlockFormControlLabel,
  BlockNavigation,
  ButtonRow,
  FormLoader,
  SectionContent,
  ValidatedTextField,
  useLayoutTitle
} from 'components';
import { toast } from 'components/toast';
import { useI18nContext } from 'i18n/i18n-react';
import type { NTPSettingsType, Time } from 'types';
import { formatLocalDateTime, updateValueDirty, useRest } from 'utils';
import { ValidationError, validate } from 'validators';
import { NTP_SETTINGS_VALIDATOR } from 'validators/ntp';
import type { ValidateFieldsError } from 'validators/schema';

import {
  TIME_ZONES,
  migrateNtpTimeZones,
  selectedTimeZone,
  timeZoneSelectItemsT,
  useTimeZoneSelectItems
} from './TZ';

const NTPSettings = () => {
  const {
    loadData,
    saving,
    data,
    updateDataValue,
    origData,
    dirtyFlags,
    setDirtyFlags,
    setOrigData,
    blocker,
    saveData,
    errorMessage
  } = useRest<NTPSettingsType>({
    read: NTPApi.readNTPSettings,
    update: NTPApi.updateNTPSettings
  });

  const { LL } = useI18nContext();
  useLayoutTitle('NTP');

  const timeZoneItems = useTimeZoneSelectItems();
  const timeZoneItemsT = timeZoneSelectItemsT();

  const selectedTzValue = data
    ? selectedTimeZone(data.tz_label, data.tz_format)
    : undefined;

  const selectedTzValueT = data
    ? selectedTimeZone(data.tz_label_t, data.tz_format_t)
    : undefined;
  const [localTime, setLocalTime] = useState<string>('');
  const [settingTime, setSettingTime] = useState<boolean>(false);
  const [processing, setProcessing] = useState<boolean>(false);
  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();
  const migratingTz = useRef(false);

  // If firmware has not yet rewritten tz_format from the label (or the user
  // landed here before reboot), persist the current TIME_ZONES values quietly.
  useEffect(() => {
    if (!data || migratingTz.current || dirtyFlags.length > 0) {
      return;
    }
    const migrated = migrateNtpTimeZones(data);
    if (!migrated) {
      return;
    }
    migratingTz.current = true;
    void (async () => {
      try {
        updateDataValue(migrated);
        await NTPApi.updateNTPSettings(migrated);
        setOrigData(migrated);
      } catch {
        // Boot-time firmware migration still applies on the next restart.
      } finally {
        migratingTz.current = false;
      }
    })();
  }, [data, dirtyFlags.length, setOrigData, updateDataValue]);

  const { send: updateTime } = useRequest(
    (local_time: Time) => NTPApi.updateTime(local_time),
    {
      immediate: false
    }
  );

  const updateFormValue = updateValueDirty(
    origData as unknown as Record<string, unknown>,
    dirtyFlags,
    setDirtyFlags,
    updateDataValue as (value: unknown) => void
  );

  const updateLocalTime = (event: React.ChangeEvent<HTMLInputElement>) =>
    setLocalTime(event.target.value);

  const openSetTime = () => {
    setLocalTime(formatLocalDateTime(new Date()));
    setSettingTime(true);
  };

  const configureTime = async () => {
    setProcessing(true);

    try {
      await updateTime({ local_time: formatLocalDateTime(new Date(localTime)) });
      toast.success(LL.TIME_SET());
      setSettingTime(false);
      await loadData();
    } catch {
      toast.error(LL.PROBLEM_UPDATING());
    } finally {
      setProcessing(false);
    }
  };

  const handleCloseSetTime = () => setSettingTime(false);

  const validateAndSubmit = async () => {
    if (!data) return;
    try {
      setFieldErrors(undefined);
      await validate(NTP_SETTINGS_VALIDATOR, data);
      await saveData();
    } catch (error) {
      setFieldErrors((error as ValidationError).fieldErrors);
    }
  };

  const changeTimeZone = (event: React.ChangeEvent<HTMLInputElement>) => {
    void updateState(readNTPSettings(), (settings: NTPSettingsType) => ({
      ...settings,
      tz_label: event.target.value,
      tz_format: TIME_ZONES[event.target.value]
    }));
    updateFormValue(event);
  };

  const changeTimeZoneT = (event: React.ChangeEvent<HTMLInputElement>) => {
    void updateState(readNTPSettings(), (settings: NTPSettingsType) => ({
      ...settings,
      tz_label_t: event.target.value,
      tz_format_t: TIME_ZONES[event.target.value]
    }));
    updateFormValue(event);
  };

  const renderContent = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage || ''} />;
    }

    return (
      <>
        <BlockFormControlLabel
          control={
            <Checkbox
              name="enabled"
              checked={data.enabled}
              onChange={updateFormValue}
            />
          }
          label={LL.ENABLE_NTP()}
        />
        <ValidatedTextField
          fieldErrors={fieldErrors || {}}
          name="server"
          label={LL.NTP_SERVER()}
          fullWidth
          variant="outlined"
          disabled={!data.enabled}
          value={data.server}
          onChange={updateFormValue}
          margin="normal"
        />
        <ValidatedTextField
          fieldErrors={fieldErrors || {}}
          name="tz_label"
          label={LL.TIME_ZONE()}
          fullWidth
          variant="outlined"
          value={selectedTzValue}
          onChange={changeTimeZone}
          margin="normal"
          select
        >
          <MenuItem disabled>{LL.TIME_ZONE()}...</MenuItem>
          {timeZoneItems}
        </ValidatedTextField>
        {data.enabled && (
          <Grid container spacing={2} rowSpacing={0}>
            <Grid>
              <ValidatedTextField
                fieldErrors={fieldErrors || {}}
                name="thermostat_sync"
                label="Sync EMS-Thermostat"
                variant="outlined"
                sx={{ width: '30ch' }}
                value={data.thermostat_sync}
                onChange={updateFormValue}
                margin="normal"
                select
              >
                <MenuItem value={0}>{LL.OFF()}</MenuItem>
                <MenuItem value={1}>{LL.ON()}</MenuItem>
                <MenuItem value={2}>
                  {LL.USE()} {LL.TIME_ZONE()}
                </MenuItem>
              </ValidatedTextField>
            </Grid>
            <Grid>
              {data.thermostat_sync === 2 && (
                <ValidatedTextField
                  fieldErrors={fieldErrors || {}}
                  name="tz_label_t"
                  label={LL.TIME_ZONE() + ' Thermostat'}
                  sx={{ width: '30ch' }}
                  variant="outlined"
                  value={selectedTzValueT}
                  onChange={changeTimeZoneT}
                  margin="normal"
                  select
                >
                  <MenuItem disabled>{LL.TIME_ZONE()}...</MenuItem>
                  {timeZoneItemsT}
                </ValidatedTextField>
              )}
            </Grid>
          </Grid>
        )}
        <Box sx={{ display: 'flex', flexWrap: 'wrap' }}>
          {!data.enabled && !dirtyFlags.length && (
            <Box sx={{ flexWrap: 'nowrap', whiteSpace: 'nowrap' }}>
              <ButtonRow>
                <Button
                  onClick={openSetTime}
                  variant="outlined"
                  color="primary"
                  startIcon={<AccessTimeIcon />}
                >
                  {LL.SET_TIME(0)}
                </Button>
              </ButtonRow>
            </Box>
          )}
        </Box>

        {dirtyFlags && dirtyFlags.length !== 0 && (
          <ButtonRow>
            <Button
              startIcon={<CancelIcon />}
              disabled={saving}
              variant="outlined"
              color="secondary"
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
      {renderContent()}
      <Dialog sx={dialogStyle} open={settingTime} onClose={handleCloseSetTime}>
        <DialogTitle>{LL.SET_TIME(1)}</DialogTitle>
        <DialogContent dividers>
          <Typography color="warning" variant="body2">
            {LL.SET_TIME_TEXT()}
          </Typography>
          <TextField
            label={LL.LOCAL_TIME(0)}
            type="datetime-local"
            value={localTime}
            onChange={updateLocalTime}
            disabled={processing}
            fullWidth
            slotProps={{
              inputLabel: {
                shrink: true
              }
            }}
          />
        </DialogContent>
        <DialogActions>
          <Button
            startIcon={<CancelIcon />}
            variant="outlined"
            onClick={handleCloseSetTime}
            color="secondary"
          >
            {LL.CANCEL()}
          </Button>
          <Button
            startIcon={<AccessTimeIcon />}
            variant="outlined"
            onClick={configureTime}
            disabled={processing}
            color="primary"
          >
            {LL.UPDATE()}
          </Button>
        </DialogActions>
      </Dialog>
    </SectionContent>
  );
};

export default NTPSettings;
