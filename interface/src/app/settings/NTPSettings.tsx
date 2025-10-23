import { useState } from 'react';
import { toast } from 'react-toastify';

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
  MenuItem,
  TextField,
  Typography
} from '@mui/material';

import * as NTPApi from 'api/ntp';
import { readNTPSettings } from 'api/ntp';

import { dialogStyle } from 'CustomTheme';
import { useRequest } from 'alova/client';
import { updateState } from 'alova/client';
import type { ValidateFieldsError } from 'async-validator';
import {
  BlockFormControlLabel,
  BlockNavigation,
  ButtonRow,
  FormLoader,
  SectionContent,
  ValidatedTextField,
  useLayoutTitle
} from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import type { NTPSettingsType, Time } from 'types';
import { formatLocalDateTime, updateValueDirty, useRest } from 'utils';
import { validate } from 'validators';
import { NTP_SETTINGS_VALIDATOR } from 'validators/ntp';

import { TIME_ZONES, selectedTimeZone, timeZoneSelectItems } from './TZ';

const NTPSettings = () => {
  const {
    loadData,
    saving,
    data,
    updateDataValue,
    origData,
    dirtyFlags,
    setDirtyFlags,
    blocker,
    saveData,
    errorMessage
  } = useRest<NTPSettingsType>({
    read: NTPApi.readNTPSettings,
    update: NTPApi.updateNTPSettings
  });

  const { LL } = useI18nContext();
  useLayoutTitle('NTP');

  const [localTime, setLocalTime] = useState<string>('');
  const [settingTime, setSettingTime] = useState<boolean>(false);
  const [processing, setProcessing] = useState<boolean>(false);

  const { send: updateTime } = useRequest(
    (local_time: Time) => NTPApi.updateTime(local_time),
    {
      immediate: false
    }
  );

  const updateFormValue = updateValueDirty(
    origData,
    dirtyFlags,
    setDirtyFlags,
    updateDataValue as (value: unknown) => void
  );

  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();

  const updateLocalTime = (event: React.ChangeEvent<HTMLInputElement>) =>
    setLocalTime(event.target.value);

  const openSetTime = () => {
    setLocalTime(formatLocalDateTime(new Date()));
    setSettingTime(true);
  };

  const configureTime = async () => {
    setProcessing(true);

    await updateTime({ local_time: formatLocalDateTime(new Date(localTime)) })
      .then(async () => {
        toast.success(LL.TIME_SET());
        setSettingTime(false);
        await loadData();
      })
      .catch(() => {
        toast.error(LL.PROBLEM_UPDATING());
      })
      .finally(() => {
        setProcessing(false);
      });
  };

  const renderSetTimeDialog = () => (
    <Dialog
      sx={dialogStyle}
      open={settingTime}
      onClose={() => setSettingTime(false)}
    >
      <DialogTitle>{LL.SET_TIME(1)}</DialogTitle>
      <DialogContent dividers>
        <Box color="warning.main" p={0} pl={0} pr={0} mt={0} mb={2}>
          <Typography variant="body2">{LL.SET_TIME_TEXT()}</Typography>
        </Box>
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
          onClick={() => setSettingTime(false)}
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
  );

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage || ''} />;
    }

    const validateAndSubmit = async () => {
      try {
        setFieldErrors(undefined);
        await validate(NTP_SETTINGS_VALIDATOR, data);
        await saveData();
      } catch (error) {
        setFieldErrors(error as ValidateFieldsError);
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
          value={selectedTimeZone(data.tz_label, data.tz_format)}
          onChange={changeTimeZone}
          margin="normal"
          select
        >
          <MenuItem disabled>{LL.TIME_ZONE()}...</MenuItem>
          {timeZoneSelectItems()}
        </ValidatedTextField>

        <Box display="flex" flexWrap="wrap">
          {!data.enabled && !dirtyFlags.length && (
            <Box flexWrap="nowrap" whiteSpace="nowrap">
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
        {renderSetTimeDialog()}

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
      {content()}
    </SectionContent>
  );
};

export default NTPSettings;
