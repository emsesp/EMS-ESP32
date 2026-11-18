import { useContext, useState } from 'react';
import { useNavigate } from 'react-router';

import { Button, Message } from 'components/ui';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';

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
    <Message
      level="info"
      actions={
        <>
          <Button
            variant="contained"
            onClick={() => void navigate('/settings/application')}
          >
            {LL.SETTINGS_OF(LL.APPLICATION())}
          </Button>
          <Button onClick={dismiss}>{LL.CLOSE()}</Button>
        </>
      }
    >
      {LL.TELNET_ENABLED()}
    </Message>
  );
};

export default TelnetBanner;
