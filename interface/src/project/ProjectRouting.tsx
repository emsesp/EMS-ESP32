import { FC } from 'react';
import { Navigate, Routes, Route } from 'react-router-dom';

import { RequireAdmin } from 'components';

import Dashboard from './Dashboard';
import Settings from './Settings';
import Help from './Help';

const ProjectRouting: FC = () => {
  return (
    <Routes>
      <Route path="dashboard/*" element={<Dashboard />} />
      <Route
        path="settings/*"
        element={
          <RequireAdmin>
            <Settings />
          </RequireAdmin>
        }
      />
      <Route path="help/*" element={<Help />} />
      <Route path="/*" element={<Navigate to="dashboard/data" />} />
    </Routes>
  );
};

export default ProjectRouting;
