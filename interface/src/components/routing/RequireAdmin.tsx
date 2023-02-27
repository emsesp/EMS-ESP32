import { FC, useContext } from 'react';
import { Navigate } from 'react-router-dom';

import { AuthenticatedContext } from 'contexts/authentication';
import { RequiredChildrenProps } from 'utils';

const RequireAdmin: FC<RequiredChildrenProps> = ({ children }) => {
  const authenticatedContext = useContext(AuthenticatedContext);
  return authenticatedContext.me.admin ? <>{children}</> : <Navigate replace to="/" />;
};

export default RequireAdmin;
