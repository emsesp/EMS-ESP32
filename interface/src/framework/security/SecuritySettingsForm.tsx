import { FC, useContext, useState } from 'react';
import { ValidateFieldsError } from 'async-validator';

import { Button } from '@mui/material';
import WarningIcon from '@mui/icons-material/Warning';
import CancelIcon from '@mui/icons-material/Cancel';

import * as SecurityApi from '../../api/security';
import { SecuritySettings } from '../../types';
import {
  ButtonRow,
  FormLoader,
  MessageBox,
  SectionContent,
  ValidatedPasswordField,
  BlockNavigation
} from '../../components';
import { SECURITY_SETTINGS_VALIDATOR, validate } from '../../validators';
import { updateValueDirty, useRest } from '../../utils';
import { AuthenticatedContext } from '../../contexts/authentication';

import { useI18nContext } from '../../i18n/i18n-react';

const SecuritySettingsForm: FC = () => {
  const { LL } = useI18nContext();

  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();
  const { loadData, saving, data, setData, origData, dirtyFlags, blocker, setDirtyFlags, saveData, errorMessage } =
    useRest<SecuritySettings>({
      read: SecurityApi.readSecuritySettings,
      update: SecurityApi.updateSecuritySettings
    });

  const authenticatedContext = useContext(AuthenticatedContext);

  const updateFormValue = updateValueDirty(origData, dirtyFlags, setDirtyFlags, setData);

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
      } catch (errors: any) {
        setFieldErrors(errors);
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
    <SectionContent title={LL.SETTINGS_OF(LL.SECURITY(1))} titleGutter>
      {blocker ? <BlockNavigation blocker={blocker} /> : null}
      {content()}
    </SectionContent>
  );
};

export default SecuritySettingsForm;
