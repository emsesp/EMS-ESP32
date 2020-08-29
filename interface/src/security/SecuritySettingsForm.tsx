import React from 'react';
import { ValidatorForm } from 'react-material-ui-form-validator';

import { Box, Typography } from '@material-ui/core';
import SaveIcon from '@material-ui/icons/Save';

import { withAuthenticatedContext, AuthenticatedContextProps } from '../authentication';
import { RestFormProps, PasswordValidator, FormActions, FormButton } from '../components';

import { SecuritySettings } from './types';

type SecuritySettingsFormProps = RestFormProps<SecuritySettings> & AuthenticatedContextProps;

class SecuritySettingsForm extends React.Component<SecuritySettingsFormProps> {

  onSubmit = () => {
    this.props.saveData();
    this.props.authenticatedContext.refresh();
  }

  render() {
    const { data, handleValueChange } = this.props;
    return (
      <ValidatorForm onSubmit={this.onSubmit}>
        <PasswordValidator
          validators={['required', 'matchRegexp:^.{1,64}$']}
          errorMessages={['JWT Secret Required', 'JWT Secret must be 64 characters or less']}
          name="jwt_secret"
          label="JWT Secret"
          fullWidth
          variant="outlined"
          value={data.jwt_secret}
          onChange={handleValueChange('jwt_secret')}
          margin="normal"
        />
        <Box bgcolor="primary.main" color="primary.contrastText" p={2} mt={2} mb={2}>
          <Typography variant="body1">
            The JWT secret is used to sign authentication tokens. If you modify the JWT Secret, all users will be signed out.
          </Typography>
        </Box>
        <FormActions>
          <FormButton startIcon={<SaveIcon />} variant="contained" color="primary" type="submit">
            Save
          </FormButton>
        </FormActions>
      </ValidatorForm>
    );
  }

}

export default withAuthenticatedContext(SecuritySettingsForm);
