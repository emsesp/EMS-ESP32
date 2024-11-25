import { useContext } from 'react';
import type { FC } from 'react';
import { Navigate } from 'react-router';

import { AuthenticatedContext } from 'contexts/authentication';
import type { RequiredChildrenProps } from 'utils';

const RequireAdmin: FC<RequiredChildrenProps> = ({ children }) => {
  const authenticatedContext = useContext(AuthenticatedContext);
  return authenticatedContext.me.admin ? (
    <>{children}</>
  ) : (
    <Navigate replace to="/" />
  );
};

export default RequireAdmin;
