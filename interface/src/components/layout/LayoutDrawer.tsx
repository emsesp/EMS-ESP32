import { memo, useContext } from 'react';
import { Link } from 'react-router';

import { type AppRoute, type MenuGroup, menuRoutes } from 'app/routes';
import { Icon, cx } from 'components/ui';
import { AuthenticatedContext } from 'contexts/authentication';
import { PROJECT_NAME } from 'env';
import { useI18nContext } from 'i18n/i18n-react';
import { updateAvailable } from 'utils';

interface LayoutDrawerProps {
  // path of the top-level route of the current page
  active: string | undefined;
}

const LayoutDrawerComponent = ({ active }: LayoutDrawerProps) => {
  const { LL } = useI18nContext();
  const { me, versions, systemName } = useContext(AuthenticatedContext);

  const item = (route: AppRoute) => {
    const label = route.menu === 'account' ? me.username : route.title(LL);
    const content = (
      <>
        <Icon name={route.icon} />
        <span className="nav-label">{label}</span>
        {route.badge === 'update' && updateAvailable(versions) && (
          <span className="badge-dot" />
        )}
      </>
    );
    if (route.admin && !me.admin) {
      return (
        <li key={route.path}>
          <span className="nav-item disabled" aria-disabled="true" title={label}>
            {content}
          </span>
        </li>
      );
    }
    const current = route.path === active;
    return (
      <li key={route.path}>
        <Link
          to={route.path}
          className={cx('nav-item', current && 'active')}
          aria-current={current ? 'page' : undefined}
          title={label}
        >
          {content}
        </Link>
      </li>
    );
  };

  const group = (name: MenuGroup) => menuRoutes(name).map(item);

  return (
    <nav className="drawer" aria-label={LL.MENU()}>
      <div className="drawer-head">
        <img src="/app/icon.png" alt="" />
        <div className="drawer-name">
          <div>{PROJECT_NAME}</div>
          {systemName && <div className="sub">{systemName}</div>}
        </div>
      </div>
      <ul className="nav">
        {group('main')}
        <li className="nav-divider" role="separator" />
        <li className="nav-group">
          <div className="nav-group-title">{LL.MODULES()}</div>
          <ul className="nav">{group('modules')}</ul>
        </li>
      </ul>
      <ul className="nav bottom">
        {group('system')}
        <li className="nav-divider" role="separator" />
        {group('account')}
      </ul>
    </nav>
  );
};

const LayoutDrawer = memo(LayoutDrawerComponent);

export default LayoutDrawer;
