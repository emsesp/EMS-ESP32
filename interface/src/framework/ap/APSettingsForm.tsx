import { FC, useState } from 'react';
import { ValidateFieldsError } from 'async-validator';
import { range } from 'lodash-es';

import { Button, Checkbox, MenuItem } from '@mui/material';
import WarningIcon from '@mui/icons-material/Warning';
import CancelIcon from '@mui/icons-material/Cancel';

import { createAPSettingsValidator, validate } from 'validators';
import {
  BlockFormControlLabel,
  ButtonRow,
  FormLoader,
  SectionContent,
  ValidatedPasswordField,
  ValidatedTextField,
  BlockNavigation
} from 'components';

import { APProvisionMode, APSettings } from 'types';
import { numberValue, updateValueDirty, useRest } from 'utils';
import * as APApi from 'api/ap';

import { useI18nContext } from 'i18n/i18n-react';

export const isAPEnabled = ({ provision_mode }: APSettings) => {
  return provision_mode === APProvisionMode.AP_MODE_ALWAYS || provision_mode === APProvisionMode.AP_MODE_DISCONNECTED;
};

const APSettingsForm: FC = () => {
  const { loadData, saving, data, setData, origData, dirtyFlags, setDirtyFlags, blocker, saveData, errorMessage } =
    useRest<APSettings>({
      read: APApi.readAPSettings,
      update: APApi.updateAPSettings
    });

  const { LL } = useI18nContext();

  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();

  const updateFormValue = updateValueDirty(origData, dirtyFlags, setDirtyFlags, setData);

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage} />;
    }

    const validateAndSubmit = async () => {
      try {
        setFieldErrors(undefined);
        await validate(createAPSettingsValidator(data), data);
        saveData();
      } catch (errors: any) {
        setFieldErrors(errors);
      }
    };

    return (
      <>
        <ValidatedTextField
          fieldErrors={fieldErrors}
          name="provision_mode"
          label={LL.AP_PROVIDE() + '...'}
          value={data.provision_mode}
          fullWidth
          select
          variant="outlined"
          onChange={updateFormValue}
          margin="normal"
        >
          <MenuItem value={APProvisionMode.AP_MODE_ALWAYS}>{LL.AP_PROVIDE_TEXT_1()}</MenuItem>
          <MenuItem value={APProvisionMode.AP_MODE_DISCONNECTED}>{LL.AP_PROVIDE_TEXT_2()}</MenuItem>
          <MenuItem value={APProvisionMode.AP_NEVER}>{LL.AP_PROVIDE_TEXT_3()}</MenuItem>
        </ValidatedTextField>
        {isAPEnabled(data) && (
          <>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="ssid"
              label={LL.ACCESS_POINT(2) + ' SSID'}
              fullWidth
              variant="outlined"
              value={data.ssid}
              onChange={updateFormValue}
              margin="normal"
            />
            <ValidatedPasswordField
              fieldErrors={fieldErrors}
              name="password"
              label={LL.ACCESS_POINT(2) + ' ' + LL.PASSWORD()}
              fullWidth
              variant="outlined"
              value={data.password}
              onChange={updateFormValue}
              margin="normal"
            />
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="channel"
              label={LL.AP_PREFERRED_CHANNEL()}
              value={numberValue(data.channel)}
              fullWidth
              select
              type="number"
              variant="outlined"
              onChange={updateFormValue}
              margin="normal"
            >
              {range(1, 14).map((i) => (
                <MenuItem key={i} value={i}>
                  {i}
                </MenuItem>
              ))}
            </ValidatedTextField>
            <BlockFormControlLabel
              control={<Checkbox name="ssid_hidden" checked={data.ssid_hidden} onChange={updateFormValue} />}
              label={LL.AP_HIDE_SSID()}
            />
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="max_clients"
              label={LL.AP_MAX_CLIENTS()}
              value={numberValue(data.max_clients)}
              fullWidth
              select
              type="number"
              variant="outlined"
              onChange={updateFormValue}
              margin="normal"
            >
              {range(1, 9).map((i) => (
                <MenuItem key={i} value={i}>
                  {i}
                </MenuItem>
              ))}
            </ValidatedTextField>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="local_ip"
              label={LL.AP_LOCAL_IP()}
              fullWidth
              variant="outlined"
              value={data.local_ip}
              onChange={updateFormValue}
              margin="normal"
            />
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="gateway_ip"
              label={LL.NETWORK_GATEWAY()}
              fullWidth
              variant="outlined"
              value={data.gateway_ip}
              onChange={updateFormValue}
              margin="normal"
            />
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="subnet_mask"
              label={LL.NETWORK_SUBNET()}
              fullWidth
              variant="outlined"
              value={data.subnet_mask}
              onChange={updateFormValue}
              margin="normal"
            />
          </>
        )}
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
    <SectionContent title={LL.SETTINGS_OF(LL.ACCESS_POINT(1))} titleGutter>
      {blocker ? <BlockNavigation blocker={blocker} /> : null}
      {content()}
    </SectionContent>
  );
};

export default APSettingsForm;
