import { useContext } from 'react';
import { useNavigate } from 'react-router';

import { callAction } from 'api/app';

import { Button, Message } from 'components/ui';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';

// shown to admins after an upgrade that invalidated the existing API access tokens
const TokensResetBanner = () => {
  const { LL } = useI18nContext();
  const navigate = useNavigate();
  const { me, versions, refreshVersions } = useContext(AuthenticatedContext);

  if (!me.admin || !versions?.tokens_reset) {
    return null;
  }

  const dismiss = async () => {
    await callAction({ action: 'dismissTokensReset' }).catch(() => undefined);
    await refreshVersions();
  };

  return (
    <Message
      level="warning"
      actions={
        <>
          <Button
            icon="key"
            color="warning"
            onClick={() => void navigate('/settings/security/apikeys')}
          >
            {LL.API_KEYS()}
          </Button>
          <Button icon="close" color="secondary" onClick={() => void dismiss()}>
            {LL.CLOSE()}
          </Button>
        </>
      }
    >
      {LL.TOKENS_RESET()}
    </Message>
  );
};

export default TokensResetBanner;
