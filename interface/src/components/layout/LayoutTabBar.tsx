import { memo } from 'react';
import { Link } from 'react-router';

import { routes } from 'app/routes';
import { Icon, cx } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';

const TABS = ['/dashboard', '/devices', '/sensors', '/status'];

interface LayoutTabBarProps {
  // path of the top-level route of the current page
  active: string | undefined;
  onMore: () => void;
}

// phones only: the most used pages, and More opens the full menu
const LayoutTabBarComponent = ({ active, onMore }: LayoutTabBarProps) => {
  const { LL } = useI18nContext();
  const tabs = TABS.map((p) => routes.find((r) => r.path === p)).filter(
    (r) => r !== undefined
  );
  const inTabs = tabs.some((r) => r.path === active);

  return (
    <nav className="tabbar" aria-label={LL.MENU()}>
      {tabs.map((r) => {
        const current = r.path === active;
        return (
          <Link
            key={r.path}
            to={r.path}
            className={cx('tab', current && 'active')}
            aria-current={current ? 'page' : undefined}
          >
            <span className="tab-icon">
              <Icon name={r.icon} />
            </span>
            <span className="tab-label">{r.title(LL)}</span>
          </Link>
        );
      })}
      <button
        type="button"
        className={cx('tab', !inTabs && 'active')}
        onClick={onMore}
      >
        <span className="tab-icon">
          <Icon name="menu" />
        </span>
        <span className="tab-label">{LL.MORE()}</span>
      </button>
    </nav>
  );
};

const LayoutTabBar = memo(LayoutTabBarComponent);

export default LayoutTabBar;
