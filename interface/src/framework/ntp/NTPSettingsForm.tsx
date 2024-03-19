import CancelIcon from '@mui/icons-material/Cancel';
import WarningIcon from '@mui/icons-material/Warning';
import { Button, Checkbox, MenuItem } from '@mui/material';
// eslint-disable-next-line import/named
import { updateState } from 'alova';
import { useState } from 'react';
import { selectedTimeZone, timeZoneSelectItems, TIME_ZONES } from './TZ';
import type { ValidateFieldsError } from 'async-validator';
import type { FC } from 'react';

import type { NTPSettings } from 'types';
import * as NTPApi from 'api/ntp';
import {
  BlockFormControlLabel,
  ButtonRow,
  FormLoader,
  SectionContent,
  ValidatedTextField,
  BlockNavigation
} from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import { updateValueDirty, useRest } from 'utils';
import { validate } from 'validators';
import { NTP_SETTINGS_VALIDATOR } from 'validators/ntp';

const NTPSettingsForm: FC = () => {
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
  } = useRest<NTPSettings>({
    read: NTPApi.readNTPSettings,
    update: NTPApi.updateNTPSettings
  });

  const { LL } = useI18nContext();

  const updateFormValue = updateValueDirty(origData, dirtyFlags, setDirtyFlags, updateDataValue);

  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage} />;
    }

    const validateAndSubmit = async () => {
      try {
        setFieldErrors(undefined);
        await validate(NTP_SETTINGS_VALIDATOR, data);
        await saveData();
      } catch (errors: any) {
        setFieldErrors(errors);
      }
    };

    const changeTimeZone = (event: React.ChangeEvent<HTMLInputElement>) => {
      updateFormValue(event);

      updateState('ntpSettings', (settings) => ({
        ...settings,
        tz_label: event.target.value,
        tz_format: TIME_ZONES[event.target.value]
      }));
    };

    return (
      <>
        <BlockFormControlLabel
          control={<Checkbox name="enabled" checked={data.enabled} onChange={updateFormValue} />}
          label={LL.ENABLE_NTP()}
        />
        <ValidatedTextField
          fieldErrors={fieldErrors}
          name="server"
          label={LL.NTP_SERVER()}
          fullWidth
          variant="outlined"
          value={data.server}
          onChange={updateFormValue}
          margin="normal"
        />
        <ValidatedTextField
          fieldErrors={fieldErrors}
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
        {dirtyFlags && dirtyFlags.length !== 0 && (
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
      {content()}
    </SectionContent>
  );
};

export default NTPSettingsForm;
