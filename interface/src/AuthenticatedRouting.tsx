import { FC, useCallback, useEffect } from 'react';
import { Navigate, Routes, Route, useNavigate, useLocation } from 'react-router-dom';
import { AxiosError } from 'axios';

import * as AuthenticationApi from 'api/authentication';
import { AXIOS } from 'api/endpoints';
import { Layout, RequireAdmin } from 'components';

import Dashboard from './project/Dashboard';
import Settings from './project/Settings';
import Help from './project/Help';

import NetworkConnection from 'framework/network/NetworkConnection';
import AccessPoint from 'framework/ap/AccessPoint';
import NetworkTime from 'framework/ntp/NetworkTime';
import Mqtt from 'framework/mqtt/Mqtt';
import System from 'framework/system/System';
import Security from 'framework/security/Security';

const AuthenticatedRouting: FC = () => {
  const location = useLocation();
  const navigate = useNavigate();

  const handleApiResponseError = useCallback(
    (error: AxiosError) => {
      if (error.response && error.response.status === 401) {
        AuthenticationApi.storeLoginRedirect(location);
        navigate('/unauthorized');
      }
      return Promise.reject(error);
    },
    [location, navigate]
  );

  useEffect(() => {
    const axiosHandlerId = AXIOS.interceptors.response.use((response) => response, handleApiResponseError);
    return () => AXIOS.interceptors.response.eject(axiosHandlerId);
  }, [handleApiResponseError]);

  return (
    <Layout>
      <Routes>
        <Route path="/dashboard/*" element={<Dashboard />} />
        <Route
          path="/settings/*"
          element={
            <RequireAdmin>
              <Settings />
            </RequireAdmin>
          }
        />
        <Route path="/help/*" element={<Help />} />

        <Route path="/network/*" element={<NetworkConnection />} />
        <Route path="/ap/*" element={<AccessPoint />} />
        <Route path="/ntp/*" element={<NetworkTime />} />
        <Route path="/mqtt/*" element={<Mqtt />} />
        <Route
          path="/security/*"
          element={
            <RequireAdmin>
              <Security />
            </RequireAdmin>
          }
        />
        <Route path="/system/*" element={<System />} />
        <Route path="/*" element={<Navigate to="/" />} />
      </Routes>
    </Layout>
  );
};

export default AuthenticatedRouting;
