import { memo, useContext } from 'react';
import { Link } from 'react-router';

import { type AppRoute, type MenuGroup, menuRoutes } from 'app/routes';
import { Icon, IconButton, cx } from 'components/ui';
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
  const { me, versions, systemName, signOut } = useContext(AuthenticatedContext);

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

  const section = (title: string, name: MenuGroup) => (
    <li className="nav-group">
      <div className="nav-group-title">{title}</div>
      <ul className="nav">{group(name)}</ul>
    </li>
  );

  const account = menuRoutes('account')[0];
  const accountCurrent = account?.path === active;

  return (
    <nav className="drawer" aria-label={LL.MENU()}>
      <div className="drawer-head">
        <img src="/app/icon.png" alt="" />
        <div className="drawer-name">
          <div>{PROJECT_NAME}</div>
          {systemName && <div className="sub">{systemName}</div>}
        </div>
        {import.meta.env.DEV && (
          <a
            href="/rest/mockData"
            target="_blank"
            rel="noreferrer"
            title="Mock Data"
            style={{ marginLeft: 'auto', color: '#ffeb3b', display: 'flex' }}
          >
            <Icon name="bug_report" label="Mock Data" />
          </a>
        )}
      </div>
      <ul className="nav">
        {group('main')}
        {section(LL.MODULES(), 'modules')}
        {section(LL.SYSTEM(0), 'system')}
      </ul>
      {account && (
        <div className="nav-user">
          <Link
            to={account.path}
            className={cx('nav-user-link', accountCurrent && 'active')}
            aria-current={accountCurrent ? 'page' : undefined}
            title={account.title(LL)}
          >
            <span className="nav-avatar" aria-hidden="true">
              {me.username.charAt(0).toUpperCase()}
            </span>
            <span className="nav-user-text">
              <span>{me.username}</span>
              <span className="cap">
                {me.admin ? LL.ADMINISTRATOR() : LL.GUEST()}
              </span>
            </span>
          </Link>
          <IconButton
            className="nav-out"
            icon="logout"
            label={LL.SIGN_OUT()}
            plain
            onClick={() => signOut(true)}
          />
        </div>
      )}
    </nav>
  );
};

const LayoutDrawer = memo(LayoutDrawerComponent);

export default LayoutDrawer;
