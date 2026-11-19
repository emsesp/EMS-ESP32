import type { FC, ReactNode } from 'react';
import { Link } from 'react-router';

import type { StatusState } from './Hero';
import Icon from './Icon';
import './LinkList.css';
import StatusDot from './StatusDot';
import { cx } from './cx';
import type { IconName } from './icons';

export interface LinkListItem {
  // a route; or href for an external page, opened in a new tab
  to?: string;
  href?: string;
  icon: IconName;
  // avatar background
  color?: string;
  title: ReactNode;
  text?: ReactNode;
  // a third, smaller line, e.g. the host of an external link
  note?: ReactNode;
  // a dot after the title, e.g. for an update
  badge?: boolean;
  // current value at the right, before the chevron, with an optional status dot
  value?: ReactNode;
  state?: StatusState | undefined;
}

export interface LinkListProps {
  items: LinkListItem[];
  // cards in a grid (default) or rows in one card
  layout?: 'grid' | 'rows';
  className?: string;
}

const LinkList: FC<LinkListProps> = ({ items, layout = 'grid', className }) => (
  <ul className={cx('linklist', layout, className)}>
    {items.map((it) => {
      const body = (
        <>
          <span
            className="avatar"
            style={it.color ? { background: it.color } : undefined}
          >
            <Icon name={it.icon} />
          </span>
          <span className="grow">
            <span className="k">
              {it.title}
              {it.badge && <span className="badge-dot" />}
            </span>
            {it.text && <span className="v">{it.text}</span>}
            {it.note && <span className="cap">{it.note}</span>}
          </span>
          {(it.value || it.state) && (
            <span className="val">
              {it.state && <StatusDot state={it.state} />}
              {it.value && <span className="txt">{it.value}</span>}
            </span>
          )}
          <Icon name={it.href ? 'open_in_new' : 'chevron_right'} className="muted" />
        </>
      );
      return (
        <li key={it.to ?? it.href}>
          {it.href ? (
            <a href={it.href} target="_blank" rel="noreferrer">
              {body}
            </a>
          ) : (
            <Link to={it.to ?? '/'}>{body}</Link>
          )}
        </li>
      );
    })}
  </ul>
);

export default LinkList;
