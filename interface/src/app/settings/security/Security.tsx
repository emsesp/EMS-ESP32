import { memo } from 'react';
import { Outlet, useMatch } from 'react-router';

import { Tab } from '@mui/material';

import { RouterTabs, useLayoutTitle } from 'components';
import { useI18nContext } from 'i18n/i18n-react';

const Security = () => {
  const { LL } = useI18nContext();
  useLayoutTitle(LL.SECURITY(0));

  const routerTab = useMatch('/settings/security/:tab')?.pathname || false;

  return (
    <>
      <RouterTabs value={routerTab}>
        <Tab value="/settings/security/users" label={LL.MANAGE_USERS()} />
        <Tab value="/settings/security/apikeys" label={LL.API_KEYS()} />
      </RouterTabs>
      <Outlet />
    </>
  );
};

export default memo(Security);
