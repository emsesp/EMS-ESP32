import { FC, useContext } from 'react';
import { Navigate, Routes, Route } from 'react-router-dom';

import { Tab } from '@mui/material';

import { useRouterTab, RouterTabs, useLayoutTitle, RequireAdmin } from '../../components';
import { AuthenticatedContext } from '../../contexts/authentication';
import UploadFileForm from './UploadFileForm';
import SystemStatusForm from './SystemStatusForm';
import OTASettingsForm from './OTASettingsForm';

import SystemLog from './SystemLog';

import { useI18nContext } from '../../i18n/i18n-react';

const System: FC = () => {
  const { LL } = useI18nContext();

  useLayoutTitle(LL.SYSTEM(0));

  const { me } = useContext(AuthenticatedContext);
  const { routerTab } = useRouterTab();

  return (
    <>
      <RouterTabs value={routerTab}>
        <Tab value="status" label={LL.STATUS_OF(LL.SYSTEM(1))} />
        <Tab value="log" label={LL.LOG_OF(LL.SYSTEM(2))} />
        <Tab value="ota" label={LL.SETTINGS_OF('OTA')} disabled={!me.admin} />
        <Tab value="upload" label={LL.UPLOAD_DOWNLOAD()} disabled={!me.admin} />
      </RouterTabs>
      <Routes>
        <Route path="status" element={<SystemStatusForm />} />
        <Route path="log" element={<SystemLog />} />
        <Route
          path="ota"
          element={
            <RequireAdmin>
              <OTASettingsForm />
            </RequireAdmin>
          }
        />
        <Route
          path="upload"
          element={
            <RequireAdmin>
              <UploadFileForm />
            </RequireAdmin>
          }
        />
        <Route path="/*" element={<Navigate replace to="status" />} />
      </Routes>
    </>
  );
};

export default System;
