import React, { FC, useContext, useState } from 'react';
import { ValidateFieldsError } from 'async-validator';

import { Box, Button, Typography } from '@mui/material';
import SaveIcon from '@mui/icons-material/Save';

import * as SecurityApi from '../../api/security';
import { ButtonRow, FormLoader, SectionContent, ValidatedPasswordField } from '../../components';
import { SECURITY_SETTINGS_VALIDATOR, validate } from '../../validators';
import { updateValue, useRest } from '../../utils';
import { SecuritySettings } from '../../types';
import { AuthenticatedContext } from '../../contexts/authentication';

const SecuritySettingsForm: FC = () => {
  const { loadData, saving, data, setData, saveData, errorMessage } = useRest<SecuritySettings>({
    read: SecurityApi.readSecuritySettings,
    update: SecurityApi.updateSecuritySettings
  });

  const authenticatedContext = useContext(AuthenticatedContext);
  const updateFormValue = updateValue(setData);

  // TODO - extend the above hook to validate the input on submit and only save to the backend if valid.
  // NB: Saving must be asserted while validation takes place
  // NB: Must also set saving to true while validating
  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();

  const validateAndSubmit = async () => {
    if (data) {
      try {
        setFieldErrors(undefined);
        await validate(SECURITY_SETTINGS_VALIDATOR, data);
        await saveData();
        await authenticatedContext.refresh();
      } catch (errors: any) {
        setFieldErrors(errors);
      }
    }
  };

  const content = () => {
    if (!data) {
      return <FormLoader loadData={loadData} errorMessage={errorMessage} />;
    }

    return (
      <>
        <ValidatedPasswordField
          fieldErrors={fieldErrors}
          name="jwt_secret"
          label="JWT Secret"
          fullWidth
          variant="outlined"
          value={data.jwt_secret}
          onChange={updateFormValue}
          margin="normal"
        />
        <Box bgcolor="primary.main" color="primary.contrastText" p={2} mt={2} mb={2}>
          <Typography variant="body1">
            The JWT secret is used to sign authentication tokens. If you modify the JWT Secret, all users will be signed
            out.
          </Typography>
        </Box>
        <ButtonRow>
          <Button
            startIcon={<SaveIcon />}
            disabled={saving}
            variant="outlined"
            color="primary"
            type="submit"
            onClick={validateAndSubmit}
          >
            Save
          </Button>
        </ButtonRow>
      </>
    );
  };

  return (
    <SectionContent title="Security Settings" titleGutter>
      {content()}
    </SectionContent>
  );
};

export default SecuritySettingsForm;
