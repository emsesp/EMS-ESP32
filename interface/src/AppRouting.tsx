import { useContext, useEffect } from 'react';

import { Route, Routes, Navigate, useLocation } from 'react-router-dom';

import { toast } from 'react-toastify';
import type { FC } from 'react';

import AuthenticatedRouting from 'AuthenticatedRouting';
import SignIn from 'SignIn';
import { RequireAuthenticated, RequireUnauthenticated } from 'components';

import { Authentication, AuthenticationContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';

interface SecurityRedirectProps {
  message: string;
  signOut?: boolean;
}

const RootRedirect: FC<SecurityRedirectProps> = ({ message, signOut }) => {
  const authenticationContext = useContext(AuthenticationContext);
  useEffect(() => {
    signOut && authenticationContext.signOut(false);
    toast.success(message);
  }, [message, signOut, authenticationContext]);
  return <Navigate to="/" />;
};

export const RemoveTrailingSlashes = () => {
  const location = useLocation();
  return (
    location.pathname.match('/.*/$') && (
      <Navigate
        to={{
          pathname: location.pathname.replace(/\/+$/, ''),
          search: location.search
        }}
      />
    )
  );
};

const AppRouting: FC = () => {
  const { LL } = useI18nContext();

  return (
    <Authentication>
      <RemoveTrailingSlashes />
      <Routes>
        <Route path="/unauthorized" element={<RootRedirect message={LL.PLEASE_SIGNIN()} signOut />} />
        <Route path="/fileUpdated" element={<RootRedirect message={LL.UPLOAD_SUCCESSFUL()} />} />
        <Route
          path="/"
          element={
            <RequireUnauthenticated>
              <SignIn />
            </RequireUnauthenticated>
          }
        />
        <Route
          path="/*"
          element={
            <RequireAuthenticated>
              <AuthenticatedRouting />
            </RequireAuthenticated>
          }
        />
      </Routes>
    </Authentication>
  );
};

export default AppRouting;
