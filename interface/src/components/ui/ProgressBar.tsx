import type { FC } from 'react';

import './ProgressBar.css';
import { cx } from './cx';

export interface ProgressBarProps {
  // 0 to 100
  value: number;
  color?: 'primary' | 'success' | 'warning' | 'error';
  label?: string;
  className?: string;
}

const ProgressBar: FC<ProgressBarProps> = ({
  value,
  color = 'primary',
  label,
  className
}) => {
  const pct = Math.max(0, Math.min(100, value));
  return (
    <div
      className={cx('bar', color, className)}
      role="progressbar"
      aria-valuenow={Math.round(pct)}
      aria-valuemin={0}
      aria-valuemax={100}
      aria-label={label}
    >
      <div style={{ width: `${pct}%` }} />
    </div>
  );
};

export default ProgressBar;
