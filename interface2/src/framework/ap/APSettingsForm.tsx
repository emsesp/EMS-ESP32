import { FC, useState } from 'react';
import { ValidateFieldsError } from 'async-validator';
import { range } from 'lodash';

import {  Button, Checkbox, MenuItem  } from '@mui/material';
import SaveIcon from '@mui/icons-material/Save';

import { createAPSettingsValidator, validate } from '../../validators';
import { BlockFormControlLabel, ButtonRow, FormLoader, SectionContent, ValidatedPasswordField, ValidatedTextField } from '../../components';
import { APProvisionMode, APSettings } from '../../types';
import { updateValue, useRest } from '../../utils';
import * as APApi from "../../api/ap";

export const isAPEnabled = ({ provision_mode }: APSettings) => {
  return provision_mode === APProvisionMode.AP_MODE_ALWAYS || provision_mode === APProvisionMode.AP_MODE_DISCONNECTED;
};

const APSettingsForm: FC = () => {

  const {
    loadData, saving, data, setData, saveData, errorMessage
  } = useRest<APSettings>({ read: APApi.readAPSettings, update: APApi.updateAPSettings });

  const updateFormValue = updateValue(setData);

  // TODO - extend the above hook to validate the input on submit and only save to the backend if valid.
  // NB: Saving must be asserted while validation takes place
  // NB: Must also set saving to true while validating
  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();

  const validateAndSubmit = async () => {
    if (data) {
      try {
        setFieldErrors(undefined);
        await validate(createAPSettingsValidator(data), data);
        saveData();
      } catch (errors: any) {
        setFieldErrors(errors);
      }
    }
  };

  const content = () => {
    if (!data) {
      return (<FormLoader loadData={loadData} errorMessage={errorMessage} />);
    }

    return (
      <>
        <ValidatedTextField
          fieldErrors={fieldErrors}
          name="provision_mode"
          label="Provide Access Point&hellip;"
          value={data.provision_mode}
          fullWidth
          select
          variant="outlined"
          onChange={updateFormValue}
          margin="normal"
        >
          <MenuItem value={APProvisionMode.AP_MODE_ALWAYS}>Always</MenuItem>
          <MenuItem value={APProvisionMode.AP_MODE_DISCONNECTED}>When WiFi Disconnected</MenuItem>
          <MenuItem value={APProvisionMode.AP_NEVER}>Never</MenuItem>
        </ValidatedTextField>
        {
          isAPEnabled(data) &&
          <>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="ssid"
              label="Access Point SSID"
              fullWidth
              variant="outlined"
              value={data.ssid}
              onChange={updateFormValue}
              margin="normal"
            />
            <ValidatedPasswordField
              fieldErrors={fieldErrors}
              name="password"
              label="Access Point Password"
              fullWidth
              variant="outlined"
              value={data.password}
              onChange={updateFormValue}
              margin="normal"
            />
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="channel"
              label="Preferred Channel"
              value={data.channel}
              fullWidth
              select
              type="number"
              variant="outlined"
              onChange={updateFormValue}
              margin="normal"
            >
              {
                range(1, 14).map((i) => <MenuItem key={i} value={i}>{i}</MenuItem>)
              }
            </ValidatedTextField>
            <BlockFormControlLabel
              control={
                <Checkbox
                  name="ssid_hidden"
                  checked={data.ssid_hidden}
                  onChange={updateFormValue}
                />
              }
              label="Hide SSID?"
            />
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="max_clients"
              label="Max Clients"
              value={data.max_clients}
              fullWidth
              select
              type="number"
              variant="outlined"
              onChange={updateFormValue}
              margin="normal"
            >
              {
                range(1, 9).map((i) => <MenuItem key={i} value={i}>{i}</MenuItem>)
              }
            </ValidatedTextField>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="local_ip"
              label="Local IP"
              fullWidth
              variant="outlined"
              value={data.local_ip}
              onChange={updateFormValue}
              margin="normal"
            />
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="gateway_ip"
              label="Gateway"
              fullWidth
              variant="outlined"
              value={data.gateway_ip}
              onChange={updateFormValue}
              margin="normal"
            />
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="subnet_mask"
              label="Subnet"
              fullWidth
              variant="outlined"
              value={data.subnet_mask}
              onChange={updateFormValue}
              margin="normal"
            />
          </>
        }
        <ButtonRow>
          <Button startIcon={<SaveIcon />} disabled={saving} variant="outlined" color="primary" type="submit" onClick={validateAndSubmit}>
            Save
          </Button>
        </ButtonRow>
      </>
    );
  };

  return (
    <SectionContent title='Access Point Settings' titleGutter>
      {content()}
    </SectionContent>
  );
};

export default APSettingsForm;
