import { FC, useState } from 'react';
import { ValidateFieldsError } from 'async-validator';

import { Button, Checkbox, MenuItem } from '@mui/material';
import SaveIcon from '@mui/icons-material/Save';
import CancelIcon from '@mui/icons-material/Cancel';

import { validate } from '../../validators';
import { BlockFormControlLabel, ButtonRow, FormLoader, SectionContent, ValidatedTextField } from '../../components';
import { NTPSettings } from '../../types';
import { updateValueDirty, useRest } from '../../utils';
import * as NTPApi from '../../api/ntp';
import { selectedTimeZone, timeZoneSelectItems, TIME_ZONES } from './TZ';
import { NTP_SETTINGS_VALIDATOR } from '../../validators/ntp';

import { useI18nContext } from '../../i18n/i18n-react';

const NTPSettingsForm: FC = () => {
  const { loadData, saving, data, setData, origData, dirtyFlags, setDirtyFlags, saveData, errorMessage } =
    useRest<NTPSettings>({
      read: NTPApi.readNTPSettings,
      update: NTPApi.updateNTPSettings
    });

  const { LL } = useI18nContext();

  const updateFormValue = updateValueDirty(origData, dirtyFlags, setDirtyFlags, setData);

  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage} />;
    }

    const validateAndSubmit = async () => {
      try {
        setFieldErrors(undefined);
        await validate(NTP_SETTINGS_VALIDATOR, data);
        saveData();
      } catch (errors: any) {
        setFieldErrors(errors);
      }
    };

    const changeTimeZone = (event: React.ChangeEvent<HTMLInputElement>) => {
      setData({
        ...data,
        tz_label: event.target.value,
        tz_format: TIME_ZONES[event.target.value]
      });
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
              onClick={() => loadData()}
            >
              {LL.CANCEL()}
            </Button>
            <Button
              startIcon={<SaveIcon />}
              disabled={saving}
              variant="outlined"
              color="primary"
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
    <SectionContent title={LL.SETTINGS_OF('NTP')} titleGutter>
      {content()}
    </SectionContent>
  );
};

export default NTPSettingsForm;
