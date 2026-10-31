import { useContext, useState } from 'react';
import { useNavigate } from 'react-router';

import { Box, Button } from '@mui/material';

import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';

import MessageBox from '../MessageBox';

const DISMISSED_KEY = 'telnetBannerDismissed';

// shown to admins while the unencrypted telnet console is enabled, until closed for this browser session
const TelnetBanner = () => {
  const { LL } = useI18nContext();
  const navigate = useNavigate();
  const { me, versions } = useContext(AuthenticatedContext);
  const [dismissed, setDismissed] = useState<boolean>(
    () => sessionStorage.getItem(DISMISSED_KEY) === 'true'
  );

  if (!me.admin || !versions?.telnet_enabled || dismissed) {
    return null;
  }

  const dismiss = () => {
    sessionStorage.setItem(DISMISSED_KEY, 'true');
    setDismissed(true);
  };

  return (
    <Box sx={{ px: 2, pt: 2 }}>
      <MessageBox level="info" message={LL.TELNET_ENABLED()}>
        <Button
          sx={{ ml: 2 }}
          variant="contained"
          color="info"
          onClick={() => void navigate('/settings/application')}
        >
          {LL.SETTINGS_OF(LL.APPLICATION())}
        </Button>
        <Button sx={{ ml: 1 }} variant="contained" color="info" onClick={dismiss}>
          {LL.CLOSE()}
        </Button>
      </MessageBox>
    </Box>
  );
};

export default TelnetBanner;
