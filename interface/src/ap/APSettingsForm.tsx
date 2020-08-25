import React, { Fragment } from 'react';
import { TextValidator, ValidatorForm, SelectValidator } from 'react-material-ui-form-validator';

import MenuItem from '@material-ui/core/MenuItem';
import SaveIcon from '@material-ui/icons/Save';

import { PasswordValidator, RestFormProps, FormActions, FormButton } from '../components';

import { isAPEnabled } from './APModes';
import { APSettings, APProvisionMode } from './types';
import { isIP } from '../validators';

type APSettingsFormProps = RestFormProps<APSettings>;

class APSettingsForm extends React.Component<APSettingsFormProps> {

  componentWillMount() {
    ValidatorForm.addValidationRule('isIP', isIP);
  }

  render() {
    const { data, handleValueChange, saveData } = this.props;
    return (
      <ValidatorForm onSubmit={saveData} ref="APSettingsForm">
        <SelectValidator name="provision_mode"
          label="Provide Access Point&hellip;"
          value={data.provision_mode}
          fullWidth
          variant="outlined"
          onChange={handleValueChange('provision_mode')}
          margin="normal">
          <MenuItem value={APProvisionMode.AP_MODE_ALWAYS}>Always</MenuItem>
          <MenuItem value={APProvisionMode.AP_MODE_DISCONNECTED}>When WiFi Disconnected</MenuItem>
          <MenuItem value={APProvisionMode.AP_NEVER}>Never</MenuItem>
        </SelectValidator>
        {
          isAPEnabled(data) &&
          <Fragment>
            <TextValidator
              validators={['required', 'matchRegexp:^.{1,32}$']}
              errorMessages={['Access Point SSID is required', 'Access Point SSID must be 32 characters or less']}
              name="ssid"
              label="Access Point SSID"
              fullWidth
              variant="outlined"
              value={data.ssid}
              onChange={handleValueChange('ssid')}
              margin="normal"
            />
            <PasswordValidator
              validators={['required', 'matchRegexp:^.{8,64}$']}
              errorMessages={['Access Point Password is required', 'Access Point Password must be 8-64 characters']}
              name="password"
              label="Access Point Password"
              fullWidth
              variant="outlined"
              value={data.password}
              onChange={handleValueChange('password')}
              margin="normal"
            />
            <TextValidator
              validators={['required', 'isIP']}
              errorMessages={['Local IP is required', 'Must be an IP address']}
              name="local_ip"
              label="Local IP"
              fullWidth
              variant="outlined"
              value={data.local_ip}
              onChange={handleValueChange('local_ip')}
              margin="normal"
            />
            <TextValidator
              validators={['required', 'isIP']}
              errorMessages={['Gateway IP is required', 'Must be an IP address']}
              name="gateway_ip"
              label="Gateway"
              fullWidth
              variant="outlined"
              value={data.gateway_ip}
              onChange={handleValueChange('gateway_ip')}
              margin="normal"
            />
            <TextValidator
              validators={['required', 'isIP']}
              errorMessages={['Subnet mask is required', 'Must be an IP address']}
              name="subnet_mask"
              label="Subnet"
              fullWidth
              variant="outlined"
              value={data.subnet_mask}
              onChange={handleValueChange('subnet_mask')}
              margin="normal"
            />
          </Fragment>
        }
        <FormActions>
          <FormButton startIcon={<SaveIcon />} variant="contained" color="primary" type="submit">
            Save
          </FormButton>
        </FormActions>
      </ValidatorForm>
    );
  }
}

export default APSettingsForm;
