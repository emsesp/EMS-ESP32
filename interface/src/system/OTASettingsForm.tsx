import React from 'react';
import { TextValidator, ValidatorForm } from 'react-material-ui-form-validator';

import { Checkbox } from '@material-ui/core';
import SaveIcon from '@material-ui/icons/Save';

import { RestFormProps, BlockFormControlLabel, PasswordValidator, FormButton, FormActions } from '../components';
import {isIP,isHostname,or}  from '../validators';

import { OTASettings } from './types';

type OTASettingsFormProps = RestFormProps<OTASettings>;

class OTASettingsForm extends React.Component<OTASettingsFormProps> {

  componentDidMount() {
    ValidatorForm.addValidationRule('isIPOrHostname', or(isIP, isHostname));
  }

  render() {
    const { data, handleValueChange, saveData } = this.props;
    return (
      <ValidatorForm onSubmit={saveData}>
        <BlockFormControlLabel
          control={
            <Checkbox
              checked={data.enabled}
              onChange={handleValueChange("enabled")}
            />
          }
          label="Enable OTA Updates?"
        />
        <TextValidator
          validators={['required', 'isNumber', 'minNumber:1025', 'maxNumber:65535']}
          errorMessages={['Port is required', "Must be a number", "Must be greater than 1024 ", "Max value is 65535"]}
          name="port"
          label="Port"
          fullWidth
          variant="outlined"
          value={data.port}
          type="number"
          onChange={handleValueChange('port')}
          margin="normal"
        />
        <PasswordValidator
          validators={['required', 'matchRegexp:^.{1,64}$']}
          errorMessages={['OTA Password is required', 'OTA Point Password must be 64 characters or less']}
          name="password"
          label="Password"
          fullWidth
          variant="outlined"
          value={data.password}
          onChange={handleValueChange('password')}
          margin="normal"
        />
        <FormActions>
          <FormButton startIcon={<SaveIcon />} variant="contained" color="primary" type="submit">
            Save
          </FormButton>
        </FormActions>
      </ValidatorForm>
    );
  }
}

export default OTASettingsForm;
