import type { FC } from 'react';
import { Navigate, Route, Routes } from 'react-router-dom';

import { Tab } from '@mui/material';

import { RouterTabs, useLayoutTitle, useRouterTab } from 'components';
import { useI18nContext } from 'i18n/i18n-react';

import APSettings from './APSettings';
import APStatus from './APStatus';

const AccessPoint: FC = () => {
  const { LL } = useI18nContext();

  useLayoutTitle(LL.ACCESS_POINT(0));

  const { routerTab } = useRouterTab();

  return (
    <>
      <RouterTabs value={routerTab}>
        <Tab value="settings" label={LL.SETTINGS_OF(LL.ACCESS_POINT(1))} />
        <Tab value="status" label={LL.STATUS_OF(LL.ACCESS_POINT(1))} />
      </RouterTabs>
      <Routes>
        <Route path="status" element={<APStatus />} />
        <Route path="settings" element={<APSettings />} />
        <Route path="*" element={<Navigate replace to="settings" />} />
      </Routes>
    </>
  );
};

export default AccessPoint;
