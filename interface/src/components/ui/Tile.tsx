import type { FC, ReactNode } from 'react';
import { Link } from 'react-router';

import type { StatusState } from './Hero';
import Icon from './Icon';
import StatusDot from './StatusDot';
import './Tile.css';
import type { IconName } from './icons';

export interface TileProps {
  to: string;
  icon: IconName;
  // avatar background
  color: string;
  title: string;
  text?: ReactNode;
  // dot at the right
  state?: StatusState | undefined;
  // yellow dot after the title, e.g. a firmware update
  badge?: boolean | undefined;
}

// a link card with an avatar, used in tile grids (wrap them in <Tiles>)
export const Tile: FC<TileProps> = ({
  to,
  icon,
  color,
  title,
  text,
  state,
  badge
}) => (
  <Link className="tile" to={to}>
    <span className="avatar" style={{ background: color }}>
      <Icon name={icon} />
    </span>
    <span className="grow">
      <span className="k">
        {title}
        {badge && <span className="tile-badge" />}
      </span>
      {text && <span className="v">{text}</span>}
    </span>
    {state && <StatusDot state={state} />}
  </Link>
);

export const Tiles: FC<{ children: ReactNode }> = ({ children }) => (
  <div className="tiles">{children}</div>
);
