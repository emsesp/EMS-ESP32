import type { FC, ReactNode } from 'react';

import './StatRow.css';
import { cx } from './cx';

export interface StatProps {
  value: ReactNode;
  label: string;
  warn?: boolean;
}

// a big number with a caption; wrap several in a StatRow
export const Stat: FC<StatProps> = ({ value, label, warn }) => (
  <div className="stat">
    <div className={cx('n', warn && 'warnc')}>{value}</div>
    <div className="cap">{label}</div>
  </div>
);

export const StatRow: FC<{ children: ReactNode }> = ({ children }) => (
  <div className="stat-row">{children}</div>
);
