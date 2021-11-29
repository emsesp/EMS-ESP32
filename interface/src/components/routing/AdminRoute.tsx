import { FC, useContext } from 'react';
import { RouteProps, Redirect, Route, RouteComponentProps } from 'react-router-dom';

import { AuthenticatedContext } from '../../contexts/authentication';

type AdminRouteProps = Omit<RouteProps, 'component' | 'render'>;

const AdminRoute: FC<AdminRouteProps> = ({ children, ...rest }) => {
  const authenticatedContext = useContext(AuthenticatedContext);
  const renderComponent: FC<RouteComponentProps<any>> = () => {
    if (authenticatedContext.me.admin) {
      return <>{children}</>;
    }
    return <Redirect to="/" />;
  };
  return <Route {...rest} render={renderComponent} />;
};

export default AdminRoute;
