import { FC } from 'react';
import { Navigate, Route, Routes } from 'react-router-dom';

import { Tab } from '@mui/material';

import { RouterTabs, useRouterTab, useLayoutTitle } from '../components';

import HelpInformation from './HelpInformation';

const Help: FC = () => {
  useLayoutTitle('Help');
  const { routerTab } = useRouterTab();

  return (
    <>
      <RouterTabs value={routerTab}>
        <Tab value="information" label="EMS-ESP Help" />
      </RouterTabs>
      <Routes>
        <Route path="information" element={<HelpInformation />} />
        <Route path="/*" element={<Navigate replace to="information" />} />
      </Routes>
    </>
  );
};

export default Help;
