import { Fragment, memo } from 'react';
import { Link } from 'react-router';

import type { AppRoute } from 'app/routes';
import { Icon, IconButton } from 'components/ui';
import { PROJECT_NAME } from 'env';
import { useI18nContext } from 'i18n/i18n-react';

interface LayoutAppBarProps {
  trail: AppRoute[];
  onToggleDrawer: () => void;
  // the element for the controls of the page (search, ...) at the right end
  actionsRef: (el: HTMLElement | null) => void;
}

// breadcrumbs from the route table; on phones only a back arrow to the parent and the title
const LayoutAppBarComponent = ({
  trail,
  onToggleDrawer,
  actionsRef
}: LayoutAppBarProps) => {
  const { LL } = useI18nContext();
  const current = trail[trail.length - 1];
  const parent = trail[trail.length - 2];

  return (
    <header className="appbar">
      <IconButton
        className="menu-btn"
        plain
        icon="menu"
        label={LL.MENU()}
        onClick={onToggleDrawer}
      />
      {parent && (
        <Link
          className="crumb-back icon-btn"
          to={parent.path}
          title={parent.title(LL)}
        >
          <Icon name="arrow_back" label={parent.title(LL)} />
        </Link>
      )}
      <h1>
        {trail.slice(0, -1).map((r) => (
          <Fragment key={r.path}>
            <Link className="crumb-link" to={r.path}>
              {r.title(LL)}
            </Link>
            <span className="crumb" aria-hidden="true">
              ›
            </span>
          </Fragment>
        ))}
        <span className="crumb-current">
          {current ? current.title(LL) : PROJECT_NAME}
        </span>
      </h1>
      <div className="appbar-actions" ref={actionsRef} />
    </header>
  );
};

const LayoutAppBar = memo(LayoutAppBarComponent);

export default LayoutAppBar;
