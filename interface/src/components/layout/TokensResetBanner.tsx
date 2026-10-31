import { useContext } from 'react';
import { useNavigate } from 'react-router';

import { Box, Button } from '@mui/material';

import { callAction } from 'api/app';

import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';

import MessageBox from '../MessageBox';

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
    <Box sx={{ px: 2, pt: 2 }}>
      <MessageBox level="warning" message={LL.TOKENS_RESET()}>
        <Button
          sx={{ ml: 2 }}
          variant="outlined"
          color="warning"
          onClick={() => void navigate('/settings/security/users')}
        >
          {LL.MANAGE_USERS()}
        </Button>
        <Button
          sx={{ ml: 1 }}
          variant="outlined"
          color="warning"
          onClick={() => void dismiss()}
        >
          {LL.CLOSE()}
        </Button>
      </MessageBox>
    </Box>
  );
};

export default TokensResetBanner;
