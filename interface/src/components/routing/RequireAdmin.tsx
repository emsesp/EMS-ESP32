import { useContext } from 'react';
import { Navigate } from 'react-router-dom';
import type { FC } from 'react';

import type { RequiredChildrenProps } from 'utils';
import { AuthenticatedContext } from 'contexts/authentication';

const RequireAdmin: FC<RequiredChildrenProps> = ({ children }) => {
  const authenticatedContext = useContext(AuthenticatedContext);
  return authenticatedContext.me.admin ? <>{children}</> : <Navigate replace to="/" />;
};

export default RequireAdmin;
