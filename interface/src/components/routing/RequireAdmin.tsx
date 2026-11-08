import { memo, useContext } from 'react';
import { Navigate, Outlet } from 'react-router';

import { AuthenticatedContext } from 'contexts/authentication';

// Layout-route guard: renders nested admin routes only for admins, otherwise
// redirects home. Must be used inside the authenticated route subtree so that
// AuthenticatedContext (and `me`) is available.
const RequireAdmin = () => {
  const { me } = useContext(AuthenticatedContext);
  return me.admin ? <Outlet /> : <Navigate replace to="/" />;
};

export default memo(RequireAdmin);
