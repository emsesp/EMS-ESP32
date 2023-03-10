import { FC, useContext, useEffect } from 'react';

import { Route, Routes, Navigate, useLocation } from 'react-router-dom';

import { toast } from 'react-toastify';

import { useI18nContext } from 'i18n/i18n-react';

import { Authentication, AuthenticationContext } from 'contexts/authentication';
import { RequireAuthenticated, RequireUnauthenticated } from 'components';

import SignIn from 'SignIn';
import AuthenticatedRouting from 'AuthenticatedRouting';

interface SecurityRedirectProps {
  message: string;
  // variant?: VariantType;
  signOut?: boolean;
}

const RootRedirect: FC<SecurityRedirectProps> = ({ message, signOut }) => {
  const authenticationContext = useContext(AuthenticationContext);
  useEffect(() => {
    signOut && authenticationContext.signOut(false);
    // TODO toast variant
    toast.error(message);
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
