import { useContext, useState } from 'react';

import CancelIcon from '@mui/icons-material/Cancel';
import WarningIcon from '@mui/icons-material/Warning';
import { Button } from '@mui/material';

import * as SecurityApi from 'api/security';

import type { ValidateFieldsError } from 'async-validator';
import {
  BlockNavigation,
  ButtonRow,
  FormLoader,
  MessageBox,
  SectionContent,
  ValidatedPasswordField
} from 'components';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import type { SecuritySettingsType } from 'types';
import { updateValueDirty, useRest } from 'utils';
import { SECURITY_SETTINGS_VALIDATOR, validate } from 'validators';

const SecuritySettings = () => {
  const { LL } = useI18nContext();

  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();
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
  } = useRest<SecuritySettingsType>({
    read: SecurityApi.readSecuritySettings,
    update: SecurityApi.updateSecuritySettings
  });

  const authenticatedContext = useContext(AuthenticatedContext);

  const updateFormValue = updateValueDirty(
    origData,
    dirtyFlags,
    setDirtyFlags,
    updateDataValue
  );

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage} />;
    }

    const validateAndSubmit = async () => {
      try {
        setFieldErrors(undefined);
        await validate(SECURITY_SETTINGS_VALIDATOR, data);
        await saveData();
        await authenticatedContext.refresh();
      } catch (error) {
        setFieldErrors(error as ValidateFieldsError);
      }
    };

    return (
      <>
        <ValidatedPasswordField
          fieldErrors={fieldErrors}
          name="jwt_secret"
          label={LL.SU_PASSWORD()}
          fullWidth
          variant="outlined"
          value={data.jwt_secret}
          onChange={updateFormValue}
          margin="normal"
        />
        <MessageBox level="info" message={LL.SU_TEXT()} mt={1} />
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

export default SecuritySettings;
