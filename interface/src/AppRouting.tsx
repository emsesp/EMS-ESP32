import { useContext, useEffect } from 'react';
import { Navigate, Route, Routes } from 'react-router';
import { toast } from 'react-toastify';

import AuthenticatedRouting from 'AuthenticatedRouting';
import SignIn from 'SignIn';
import { RequireAuthenticated, RequireUnauthenticated } from 'components';
import { Authentication, AuthenticationContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';

interface SecurityRedirectProps {
  message: string;
  signOut?: boolean;
}

const RootRedirect = ({ message, signOut }: SecurityRedirectProps) => {
  const authenticationContext = useContext(AuthenticationContext);
  useEffect(() => {
    signOut && authenticationContext.signOut(false);
    toast.success(message);
  }, [message, signOut, authenticationContext]);
  return <Navigate to="/" />;
};

const AppRouting = () => {
  const { LL } = useI18nContext();

  return (
    <Authentication>
      <Routes>
        <Route
          path="/unauthorized"
          element={<RootRedirect message={LL.PLEASE_SIGNIN()} signOut />}
        />
        <Route
          path="/fileUpdated"
          element={<RootRedirect message={LL.UPLOAD_SUCCESSFUL()} />}
        />
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
