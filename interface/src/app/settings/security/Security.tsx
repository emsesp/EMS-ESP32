import { memo } from 'react';
import { Outlet } from 'react-router';

import { TabNav } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';

import './Security.css';

const Security = () => {
  const { LL } = useI18nContext();

  return (
    <>
      <TabNav
        label={LL.SECURITY(0)}
        items={[
          { to: '/settings/security/users', label: LL.USERS(), icon: 'group' },
          { to: '/settings/security/apikeys', label: LL.API_KEYS(), icon: 'key' }
        ]}
      />
      <Outlet />
    </>
  );
};

export default memo(Security);
