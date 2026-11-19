import type { FC } from 'react';
import { NavLink } from 'react-router';

import Icon from './Icon';
import './TabNav.css';
import type { IconName } from './icons';

export interface TabNavItem {
  to: string;
  label: string;
  icon?: IconName;
}

// tabs below the app bar that switch between sub-routes
const TabNav: FC<{ items: TabNavItem[]; label?: string }> = ({ items, label }) => (
  <nav className="tabnav" aria-label={label}>
    {items.map((t) => (
      <NavLink
        key={t.to}
        to={t.to}
        end
        className={({ isActive }) => (isActive ? 'on' : undefined)}
      >
        {t.icon && <Icon name={t.icon} size="sm" />}
        {t.label}
      </NavLink>
    ))}
  </nav>
);

export default TabNav;
